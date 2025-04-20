
#include <DummyConcurrency/Future/Combinators.hpp>
#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Make.hpp>
#include <DummyConcurrency/Future/Terminate.hpp>
#include <DummyConcurrency/Scheduler/RunLoop.hpp>
#include <DummyConcurrency/Scheduler/ThreadPool.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <tuple>

using namespace DummyConcurrency;

using namespace std::chrono_literals;

std::error_code TimeoutError() {
    return std::make_error_code(std::errc::timed_out);
}

std::error_code IoError() {
    return std::make_error_code(std::errc::io_error);
}

struct MoveOnly {
    MoveOnly() = default;

    // Movable
    MoveOnly(MoveOnly&&)            = default;
    MoveOnly& operator=(MoveOnly&&) = default;

    // Non-copyable
    MoveOnly(const MoveOnly&)            = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
};

struct NonDefaultConstructible {
    NonDefaultConstructible(int) {};
};

TEST_SUITE(Futures) {
    SIMPLE_TEST(ContractValue) {
        auto [f, p] = Future::Contract<std::string>();

        std::move(p).Set("Hi");
        std::string s = Future::Get(std::move(f));

        ASSERT_EQ(s, "Hi");
    }

    SIMPLE_TEST(ContractDetach) {
        auto [f, p] = Future::Contract<int>();

        std::move(f) | Future::Detach();
        std::move(p).Set(1);
    }

    SIMPLE_TEST(ContractMoveOnlyValue) {
        auto [f, p] = Future::Contract<MoveOnly>();

        std::move(p).Set(MoveOnly{});
        auto v = Future::Get(std::move(f));

        WHEELS_UNUSED(v);
    }

    SIMPLE_TEST(ContractNonDefaultConstructibleValue) {
        auto [f, p] = Future::Contract<NonDefaultConstructible>();

        std::move(p).Set({128});
        Future::Get(std::move(f));
    }

    SIMPLE_TEST(Value) {
        auto f = Future::Value(std::string("Hello"));
        auto s = Future::Get(std::move(f));

        ASSERT_EQ(s, "Hello");
    }

    SIMPLE_TEST(Just) {
        auto j = Future::Just();
        auto u = Future::Get(std::move(j));
        ASSERT_EQ(u, Unit());
    }

    SIMPLE_TEST(SubmitPool) {
        Scheduler::ThreadPool pool{4};
        pool.Start();

        auto compute = Future::Submit(pool, [] -> int { return 11; });

        int v = Future::Get(std::move(compute));

        ASSERT_EQ(v, 11);

        pool.Stop();
    }

    SIMPLE_TEST(SubmitLoop) {
        Scheduler::RunLoop loop;

        bool done = false;

        auto f = Future::Submit(loop, [&] {
            done = true;
            return Unit();
        });

        std::move(f) | Future::Detach();

        ASSERT_FALSE(done);

        loop.Run();

        ASSERT_TRUE(done);
    }

    SIMPLE_TEST(SubmitPoolWait) {
        Scheduler::ThreadPool pool{4};
        pool.Start();

        auto f = Future::Submit(pool, [] {
            std::this_thread::sleep_for(1s);
            return 11;
        });

        wheels::ProcessCPUTimer timer;

        auto v = Future::Get(std::move(f));

        ASSERT_TRUE(timer.Spent() < 100ms);

        ASSERT_EQ(v, 11);

        pool.Stop();
    }

    SIMPLE_TEST(Map) {
        auto f = Future::Value(1) | Future::Map([](int v) { return v + 1; });

        auto v = std::move(f) | Future::Get();

        ASSERT_EQ(v, 2);
    }

    SIMPLE_TEST(Flatten) {
        Scheduler::RunLoop loop;

        auto ff = Future::Submit(loop, [&loop] { return Future::Submit(loop, [] { return 7; }); });

        Future::Future<int> f = std::move(ff) | Future::Flatten();

        bool ok = false;

        std::move(f) | Future::Map([&ok](int v) -> Unit {
            ASSERT_EQ(v, 7);
            ok = true;
            return {};
        }) | Future::Detach();

        ASSERT_FALSE(ok);

        loop.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(FlatMap) {
        Scheduler::RunLoop loop;

        Future::Future<int> f = Future::Submit(loop, [] { return 23; }) |
                                Future::FlatMap([&](int v) { return Future::Submit(loop, [v] { return v + 5; }); }) |
                                Future::Map([](int v) { return v + 1; });

        bool ok = true;

        std::move(f) | Future::Map([&](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return Unit();
        }) | Future::Detach();

        loop.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(MapOk) {
        {
            auto f = Future::Ok(1) | Future::MapOk([](int v) { return v + 1; });

            auto r = Future::Get(std::move(f));
            ASSERT_TRUE(r);
            ASSERT_EQ(*r, 2);
        }

        {
            auto failure = [] -> Future::TryFuture<int> { return Future::Error(TimeoutError()); };

            auto f = failure() | Future::MapOk([](int) {
                         FAIL_TEST("Unreachable");
                         return Unit();
                     });

            auto r = Future::Get(std::move(f));
            ASSERT_FALSE(r);
        }
    }

    SIMPLE_TEST(AndThen) {
        auto f = Future::Ok(std::string("ok")) | Future::AndThen([](std::string s) { return Future::Ok(s + "!"); }) |
                 Future::AndThen([](std::string s) { return Future::Ok(s + "!"); });

        auto r = Future::Get(std::move(f));

        ASSERT_TRUE(r);
        ASSERT_EQ(*r, "ok!!");
    }

    SIMPLE_TEST(OrElse) {
        auto failure = [] -> Future::TryFuture<std::string> { return Future::Error(IoError()); };

        auto f = failure() | Future::OrElse([](Error e) {
                     ASSERT_EQ(e, IoError());
                     return Future::Ok(std::string("fallback"));
                 });

        auto r = Future::Get(std::move(f));

        ASSERT_TRUE(r);
        ASSERT_EQ(*r, "fallback");
    }

    SIMPLE_TEST(TryPipeline) {
        auto [f, p] = Future::Contract<int>();

        auto g = std::move(f) | Future::Map([](int v) { return v + 1; }) | Future::Map([](int v) { return v + 2; }) | Future::AsTryFuture() |
                 Future::OrElse([](Error) {
                     FAIL_TEST("Unreachable");
                     return Future::Ok(111);
                 }) |
                 Future::AndThen([](int /*v*/) -> Future::TryFuture<int> { return Future::Error(TimeoutError()); }) | Future::MapOk([](int v) {
                     FAIL_TEST("Unreachable");
                     return v + 1;
                 }) |
                 Future::OrElse([](Error) { return Future::Ok(17); }) | Future::MapOk([](int v) { return v + 1; });

        std::move(p).Set(3);

        auto r = Future::Get(std::move(g));

        ASSERT_TRUE(r);
        ASSERT_EQ(*r, 18);
    }

    SIMPLE_TEST(Via) {
        Scheduler::RunLoop loop1;
        Scheduler::RunLoop loop2;

        size_t steps = 0;

        Future::Just() | Future::Via(loop1) | Future::Map([&](Unit) {
            ++steps;
            return Unit();
        }) | Future::Map([](Unit) { return Unit(); }) |
            Future::Via(loop2) | Future::Map([&](Unit) {
                ++steps;
                return Unit();
            }) |
            Future::Map([&](Unit) {
                ++steps;
                return Unit();
            }) |
            Future::Via(loop1) | Future::Map([&](Unit) {
                ++steps;
                return Unit();
            }) |
            Future::FlatMap([&](Unit) {
                ++steps;
                return Future::Value(1);
            }) |
            Future::Map([&](int v) {
                ASSERT_EQ(v, 1);
                ++steps;
                return Unit();
            }) |
            Future::Detach();

        ASSERT_EQ(loop1.Run(), 2);
        ASSERT_EQ(steps, 1);
        ASSERT_GE(loop2.Run(), 2);
        ASSERT_EQ(steps, 3);
        ASSERT_GE(loop1.Run(), 3);
        ASSERT_EQ(steps, 6);
    }

    SIMPLE_TEST(First1) {
        auto [f1, p1] = Future::Contract<int>();
        auto [f2, p2] = Future::Contract<int>();

        auto first = Future::First(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | Future::Map([&ok](int v) {
            ASSERT_EQ(v, 1);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p1).Set(1);

        ASSERT_TRUE(ok);

        std::move(p2).Set(2);
    }

    SIMPLE_TEST(First2) {
        auto [f1, p1] = Future::Contract<int>();
        auto [f2, p2] = Future::Contract<int>();

        auto first = Future::First(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | Future::Map([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set(2);

        ASSERT_TRUE(ok);

        std::move(p1).Set(1);
    }

    SIMPLE_TEST(FirstOk1) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto first = Future::FirstOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | Future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set(ResultOk(2));

        ASSERT_TRUE(ok);

        std::move(p1).Set(ResultOk(1));
    }

    SIMPLE_TEST(FirstOk2) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto first = Future::FirstOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | Future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p1).Set(ResultError(TimeoutError()));
        std::move(p2).Set(ResultOk(29));

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(FirstOk3) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto first = Future::FirstOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | Future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set(ResultError(IoError()));
        std::move(p1).Set(ResultOk(31));

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(FirstFailure) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto first = Future::FirstOk(std::move(f1), std::move(f2));

        bool fail = false;

        std::move(first) | Future::OrElse([&](Error e) -> Future::TryFuture<int> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return Future::Error(e);
        }) | Future::Detach();

        std::move(p2).Set(ResultError(TimeoutError()));
        std::move(p1).Set(ResultError(TimeoutError()));

        ASSERT_TRUE(fail);
    }

    SIMPLE_TEST(Both) {
        auto [f1, p1] = Future::Contract<int>();
        auto [f2, p2] = Future::Contract<int>();

        auto both = Future::Both(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(both) | Future::Map([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 1);
            ASSERT_EQ(y, 2);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set(2);
        std::move(p1).Set(1);

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(BothOk) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto both = Future::BothOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(both) | Future::MapOk([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 2);
            ASSERT_EQ(y, 1);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set(ResultOk(1));
        std::move(p1).Set(ResultOk(2));

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(BothFailure1) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto both = Future::BothOk(std::move(f1), std::move(f2));

        bool fail = false;

        std::move(both) | Future::OrElse([&fail](Error e) -> Future::TryFuture<std::tuple<int, int>> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return Future::Error(e);
        }) | Future::Detach();

        std::move(p1).Set(ResultError(TimeoutError()));

        ASSERT_TRUE(fail);

        std::move(p2).Set(ResultOk(7));
    }

    SIMPLE_TEST(BothFailure2) {
        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto both = Future::BothOk(std::move(f1), std::move(f2));

        bool fail = false;

        std::move(both) | Future::OrElse([&fail](Error e) -> Future::TryFuture<std::tuple<int, int>> {
            ASSERT_EQ(e, IoError());
            fail = true;
            return Future::Error(e);
        }) | Future::Detach();

        std::move(p2).Set(ResultError(IoError()));

        ASSERT_TRUE(fail);

        std::move(p1).Set(ResultOk(4));
    }

    SIMPLE_TEST(BothTypes) {
        auto [f1, p1] = Future::Contract<std::string>();
        auto [f2, p2] = Future::Contract<std::tuple<int, int>>();

        auto both = Future::Both(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(both) | Future::Map([&ok](auto tuple) {
            auto [x, y] = tuple;

            ASSERT_EQ(x, "3");

            std::tuple<int, int> t = {1, 2};
            ASSERT_EQ(y, t);

            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set({1, 2});
        std::move(p1).Set("3");

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(DoNotWait1) {
        Scheduler::ThreadPool pool{4};
        pool.Start();

        std::atomic_bool submit = false;

        auto f = Future::Submit(pool,
                                [&] {
                                    std::this_thread::sleep_for(1s);
                                    submit = true;
                                    return 11;
                                }) |
                 Future::Map([](int v) { return v + 1; });

        ASSERT_FALSE(submit);

        auto v = Future::Get(std::move(f));

        ASSERT_TRUE(submit);
        ASSERT_EQ(v, 12);

        pool.Stop();
    }

    SIMPLE_TEST(DoNotWait2) {
        Scheduler::ThreadPool pool{4};
        pool.Start();

        std::atomic_bool submit = false;

        auto f = Future::Submit(pool,
                                [&] {
                                    std::this_thread::sleep_for(1s);
                                    submit = true;
                                    return 31;
                                }) |
                 Future::FlatMap([&](int v) { return Future::Submit(pool, [v] { return v + 1; }); });

        ASSERT_FALSE(submit);

        auto v = Future::Get(std::move(f));

        ASSERT_TRUE(submit);
        ASSERT_EQ(v, 32);

        pool.Stop();
    }

    SIMPLE_TEST(Inline1) {
        Scheduler::RunLoop loop;

        bool ok = false;

        Future::Just() | Future::Via(loop) | Future::Map([&](Unit) {
            ok = true;
            return Unit();
        }) | Future::Detach();

        size_t tasks = loop.Run();
        ASSERT_TRUE(ok);
        ASSERT_EQ(tasks, 1);
    }

    SIMPLE_TEST(Inline2) {
        Scheduler::RunLoop loop;

        auto v = Future::Get(Future::Value(1) | Future::Via(loop));

        ASSERT_EQ(v, 1);
    }

    SIMPLE_TEST(Inline3) {
        Scheduler::RunLoop loop;

        bool flat_map = false;
        bool map1     = false;
        bool map2     = false;

        Future::Just() | Future::Via(loop) | Future::FlatMap([&](Unit) {
            flat_map = true;
            return Future::Value(Unit());
        }) | Future::Map([&](Unit u) {
            map1 = true;
            return u;
        }) | Future::Map([&](Unit u) {
            map2 = true;
            return u;
        }) | Future::Detach();

        ASSERT_TRUE(loop.RunNext());
        ASSERT_TRUE(flat_map);
        ASSERT_FALSE(map1);

        ASSERT_TRUE(loop.RunNext());
        ASSERT_TRUE(map1);
        ASSERT_FALSE(map2);

        ASSERT_EQ(loop.Run(), 1);
        ASSERT_TRUE(map2);
    }

    SIMPLE_TEST(Inline4) {
        Scheduler::RunLoop loop;

        Future::Submit(loop, [&] { return Future::Submit(loop, [] { return 19; }); }) | Future::Flatten() | Future::Detach();

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, 2);
    }

    SIMPLE_TEST(Inline5) {
        Scheduler::RunLoop loop;

        auto [f1, p1] = Future::Contract<Result<int>>();
        auto [f2, p2] = Future::Contract<Result<int>>();

        auto first = Future::FirstOk(std::move(f1) | Future::Via(loop), std::move(f2) | Future::Via(loop));

        bool ok = false;

        std::move(first) | Future::MapOk([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return Unit();
        }) | Future::Detach();

        std::move(p2).Set(ResultError(IoError()));
        std::move(p1).Set(ResultOk(31));

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, 0);

        ASSERT_TRUE(ok);
    }
}
