
#include <DummyConcurrency/Runtime/Scheduler/RunLoop.hpp>
#include <DummyConcurrency/Runtime/Scheduler/ThreadPool.hpp>
#include <DummyConcurrency/Syntax/Syntax.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <string>
#include <thread>
#include <tuple>

using namespace NDummyConcurrency;

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
    NonDefaultConstructible(int) {};  // NOLINT
};

TEST_SUITE(Futures) {
    SIMPLE_TEST(ContractValue) {
        auto [f, p] = NFuture::Contract<std::string>();

        std::move(p).Set("Hi");
        std::string s = NFuture::Get(std::move(f));

        ASSERT_EQ(s, "Hi");
    }

    SIMPLE_TEST(ContractDetach) {
        auto [f, p] = NFuture::Contract<int>();

        std::move(f) | NFuture::Detach();
        std::move(p).Set(1);
    }

    SIMPLE_TEST(ContractMoveOnlyValue) {
        auto [f, p] = NFuture::Contract<MoveOnly>();

        std::move(p).Set(MoveOnly{});
        auto v = NFuture::Get(std::move(f));

        WHEELS_UNUSED(v);
    }

    SIMPLE_TEST(ContractNonDefaultConstructibleValue) {
        auto [f, p] = NFuture::Contract<NonDefaultConstructible>();

        std::move(p).Set({128});
        NFuture::Get(std::move(f));
    }

    SIMPLE_TEST(Value) {
        auto f = NFuture::Value(std::string("Hello"));
        auto s = NFuture::Get(std::move(f));

        ASSERT_EQ(s, "Hello");
    }

    SIMPLE_TEST(Just) {
        auto j = NFuture::Just();
        auto u = NFuture::Get(std::move(j));
        ASSERT_EQ(u, Unit());
    }

    SIMPLE_TEST(SubmitPool) {
        NRuntime::ThreadPool pool{4};
        pool.Start();

        auto compute = NFuture::Submit(pool, [] -> int { return 11; });

        int v = NFuture::Get(std::move(compute));

        ASSERT_EQ(v, 11);

        pool.Stop();
    }

    SIMPLE_TEST(SubmitLoop) {
        NRuntime::RunLoop loop;

        bool done = false;

        auto f = NFuture::Submit(loop, [&] {
            done = true;
            return Unit();
        });

        std::move(f) | NFuture::Detach();

        ASSERT_FALSE(done);

        loop.Run();

        ASSERT_TRUE(done);
    }

    SIMPLE_TEST(SubmitPoolWait) {
        NRuntime::ThreadPool pool{4};
        pool.Start();

        auto f = NFuture::Submit(pool, [] {
            std::this_thread::sleep_for(1s);
            return 11;
        });

        wheels::ProcessCPUTimer timer;

        auto v = NFuture::Get(std::move(f));

        ASSERT_TRUE(timer.Spent() < 100ms);

        ASSERT_EQ(v, 11);

        pool.Stop();
    }

    SIMPLE_TEST(Map) {
        auto f = NFuture::Value(1) | NFuture::Map([](int v) { return v + 1; });

        auto v = std::move(f) | NFuture::Get();

        ASSERT_EQ(v, 2);
    }

    SIMPLE_TEST(Flatten) {
        NRuntime::RunLoop loop;

        auto ff = NFuture::Submit(loop, [&loop] { return NFuture::Submit(loop, [] { return 7; }); });

        Future<int> f = std::move(ff) | NFuture::Flatten();

        bool ok = false;

        std::move(f) | NFuture::Map([&ok](int v) -> Unit {
            ASSERT_EQ(v, 7);
            ok = true;
            return {};
        }) | NFuture::Detach();

        ASSERT_FALSE(ok);

        loop.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(FlatMap) {
        NRuntime::RunLoop loop;

        Future<int> f = NFuture::Submit(loop, [] { return 23; }) |
                        NFuture::FlatMap([&](int v) { return NFuture::Submit(loop, [v] { return v + 5; }); }) |
                        NFuture::Map([](int v) { return v + 1; });

        bool ok = true;

        std::move(f) | NFuture::Map([&](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        loop.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(MapOk) {
        {
            auto f = NFuture::Ok(1) | NFuture::MapOk([](int v) { return v + 1; });

            auto r = NFuture::Get(std::move(f));
            ASSERT_TRUE(r);
            ASSERT_EQ(*r, 2);
        }

        {
            auto failure = [] -> NFuture::TryFuture<int> { return NFuture::Failure(TimeoutError()); };

            auto f = failure() | NFuture::MapOk([](int) {
                         FAIL_TEST("Unreachable");
                         return Unit();
                     });

            auto r = NFuture::Get(std::move(f));
            ASSERT_FALSE(r);
        }
    }

    SIMPLE_TEST(AndThen) {
        auto f = NFuture::Ok(std::string("ok")) | NFuture::AndThen([](std::string s) { return NFuture::Ok(s + "!"); }) |
                 NFuture::AndThen([](std::string s) { return NFuture::Ok(s + "!"); });

        auto r = NFuture::Get(std::move(f));

        ASSERT_TRUE(r);
        ASSERT_EQ(*r, "ok!!");
    }

    SIMPLE_TEST(OrElse) {
        auto failure = [] -> NFuture::TryFuture<std::string> { return NFuture::Failure(IoError()); };

        auto f = failure() | NFuture::OrElse([](Error e) {
                     ASSERT_EQ(e, IoError());
                     return NFuture::Ok(std::string("fallback"));
                 });

        auto r = NFuture::Get(std::move(f));

        ASSERT_TRUE(r);
        ASSERT_EQ(*r, "fallback");
    }

    SIMPLE_TEST(TryPipeline) {
        auto [f, p] = NFuture::Contract<int>();

        auto g = std::move(f) |                               //
                 NFuture::Map([](int v) { return v + 1; }) |  //
                 NFuture::Map([](int v) { return v + 2; }) |  //
                 NFuture::AsTryFuture() |                     //
                 NFuture::OrElse([](Error) {
                     FAIL_TEST("Unreachable");
                     return NFuture::Ok(111);
                 }) |  //
                 NFuture::AndThen([](int /*v*/) -> NFuture::TryFuture<int> { return NFuture::Failure(TimeoutError()); }) |
                 NFuture::MapOk([](int v) {
                     FAIL_TEST("Unreachable");
                     return v + 1;
                 }) |  //
                 NFuture::OrElse([](Error) { return NFuture::Ok(17); }) |
                 NFuture::MapOk([](int v) { return v + 1; });

        std::move(p).Set(3);

        auto r = NFuture::Get(std::move(g));

        ASSERT_TRUE(r);
        ASSERT_EQ(*r, 18);
    }

    SIMPLE_TEST(Via) {
        NRuntime::RunLoop loop1;
        NRuntime::RunLoop loop2;

        size_t steps = 0;

        NFuture::Just() | NFuture::Via(loop1) | NFuture::Map([&](Unit) {
            ++steps;
            return Unit();
        }) | NFuture::Map([](Unit) { return Unit(); }) |
            NFuture::Via(loop2) | NFuture::Map([&](Unit) {
                ++steps;
                return Unit();
            }) |
            NFuture::Map([&](Unit) {
                ++steps;
                return Unit();
            }) |
            NFuture::Via(loop1) | NFuture::Map([&](Unit) {
                ++steps;
                return Unit();
            }) |
            NFuture::FlatMap([&](Unit) {
                ++steps;
                return NFuture::Value(1);
            }) |
            NFuture::Map([&](int v) {
                ASSERT_EQ(v, 1);
                ++steps;
                return Unit();
            }) |
            NFuture::Detach();

        ASSERT_EQ(loop1.Run(), 2);
        ASSERT_EQ(steps, 1);
        ASSERT_GE(loop2.Run(), 2);
        ASSERT_EQ(steps, 3);
        ASSERT_GE(loop1.Run(), 3);
        ASSERT_EQ(steps, 6);
    }

    SIMPLE_TEST(First1) {
        auto [f1, p1] = NFuture::Contract<int>();
        auto [f2, p2] = NFuture::Contract<int>();

        auto first = std::move(f1) || std::move(f2);

        bool ok = false;

        std::move(first) | NFuture::Map([&ok](int v) {
            ASSERT_EQ(v, 1);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p1).Set(1);

        ASSERT_TRUE(ok);

        std::move(p2).Set(2);
    }

    SIMPLE_TEST(First2) {
        auto [f1, p1] = NFuture::Contract<int>();
        auto [f2, p2] = NFuture::Contract<int>();

        auto first = std::move(f1) || std::move(f2);

        bool ok = false;

        std::move(first) | NFuture::Map([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set(2);

        ASSERT_TRUE(ok);

        std::move(p1).Set(1);
    }

    SIMPLE_TEST(FirstOk1) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto first = NFuture::FirstOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | NFuture::MapOk([&ok](int v) {
            ASSERT_EQ(v, 2);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set(NResult::Ok(2));

        ASSERT_TRUE(ok);

        std::move(p1).Set(NResult::Ok(1));
    }

    SIMPLE_TEST(FirstOk2) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto first = NFuture::FirstOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | NFuture::MapOk([&ok](int v) {
            ASSERT_EQ(v, 29);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p1).Set(NResult::Failure(TimeoutError()));
        std::move(p2).Set(NResult::Ok(29));

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(FirstOk3) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto first = NFuture::FirstOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(first) | NFuture::MapOk([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set(NResult::Failure(IoError()));
        std::move(p1).Set(NResult::Ok(31));

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(FirstFailure) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto first = NFuture::FirstOk(std::move(f1), std::move(f2));

        bool fail = false;

        std::move(first) | NFuture::OrElse([&](Error e) -> NFuture::TryFuture<int> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return NFuture::Failure(e);
        }) | NFuture::Detach();

        std::move(p2).Set(NResult::Failure(TimeoutError()));
        std::move(p1).Set(NResult::Failure(TimeoutError()));

        ASSERT_TRUE(fail);
    }

    SIMPLE_TEST(Both) {
        auto [f1, p1] = NFuture::Contract<int>();
        auto [f2, p2] = NFuture::Contract<int>();

        auto both = std::move(f1) + std::move(f2);

        bool ok = false;

        std::move(both) | NFuture::Map([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 1);
            ASSERT_EQ(y, 2);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set(2);
        std::move(p1).Set(1);

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(BothOk) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto both = NFuture::BothOk(std::move(f1), std::move(f2));

        bool ok = false;

        std::move(both) | NFuture::MapOk([&ok](auto tuple) {
            auto [x, y] = tuple;
            ASSERT_EQ(x, 2);
            ASSERT_EQ(y, 1);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set(NResult::Ok(1));
        std::move(p1).Set(NResult::Ok(2));

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(BothFailure1) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto both = NFuture::BothOk(std::move(f1), std::move(f2));

        bool fail = false;

        std::move(both) | NFuture::OrElse([&fail](Error e) -> NFuture::TryFuture<std::tuple<int, int>> {
            ASSERT_EQ(e, TimeoutError());
            fail = true;
            return NFuture::Failure(e);
        }) | NFuture::Detach();

        std::move(p1).Set(NResult::Failure(TimeoutError()));

        ASSERT_TRUE(fail);

        std::move(p2).Set(NResult::Ok(7));
    }

    SIMPLE_TEST(BothFailure2) {
        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto both = NFuture::BothOk(std::move(f1), std::move(f2));

        bool fail = false;

        std::move(both) | NFuture::OrElse([&fail](Error e) -> NFuture::TryFuture<std::tuple<int, int>> {
            ASSERT_EQ(e, IoError());
            fail = true;
            return NFuture::Failure(e);
        }) | NFuture::Detach();

        std::move(p2).Set(NResult::Failure(IoError()));

        ASSERT_TRUE(fail);

        std::move(p1).Set(NResult::Ok(4));
    }

    SIMPLE_TEST(BothTypes) {
        auto [f1, p1] = NFuture::Contract<std::string>();
        auto [f2, p2] = NFuture::Contract<std::tuple<int, int>>();

        auto both = std::move(f1) + std::move(f2);

        bool ok = false;

        std::move(both) | NFuture::Map([&ok](auto tuple) {
            auto [x, y] = tuple;

            ASSERT_EQ(x, "3");

            std::tuple<int, int> t = {1, 2};
            ASSERT_EQ(y, t);

            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set({1, 2});
        std::move(p1).Set("3");

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(DoNotWait1) {
        NRuntime::ThreadPool pool{4};
        pool.Start();

        std::atomic_bool submit = false;

        auto f = NFuture::Submit(pool,
                                 [&] {
                                     std::this_thread::sleep_for(1s);
                                     submit = true;
                                     return 11;
                                 }) |
                 NFuture::Map([](int v) { return v + 1; });

        ASSERT_FALSE(submit);

        auto v = NFuture::Get(std::move(f));

        ASSERT_TRUE(submit);
        ASSERT_EQ(v, 12);

        pool.Stop();
    }

    SIMPLE_TEST(DoNotWait2) {
        NRuntime::ThreadPool pool{4};
        pool.Start();

        std::atomic_bool submit = false;

        auto f = NFuture::Submit(pool,
                                 [&] {
                                     std::this_thread::sleep_for(1s);
                                     submit = true;
                                     return 31;
                                 }) |
                 NFuture::FlatMap([&](int v) { return NFuture::Submit(pool, [v] { return v + 1; }); });

        ASSERT_FALSE(submit);

        auto v = NFuture::Get(std::move(f));

        ASSERT_TRUE(submit);
        ASSERT_EQ(v, 32);

        pool.Stop();
    }

    SIMPLE_TEST(Inline1) {
        NRuntime::RunLoop loop;

        bool ok = false;

        NFuture::Just() | NFuture::Via(loop) | NFuture::Map([&](Unit) {
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        size_t tasks = loop.Run();
        ASSERT_TRUE(ok);
        ASSERT_EQ(tasks, 1);
    }

    SIMPLE_TEST(Inline2) {
        NRuntime::RunLoop loop;

        auto v = NFuture::Get(NFuture::Value(1) | NFuture::Via(loop));

        ASSERT_EQ(v, 1);
    }

    SIMPLE_TEST(Inline3) {
        NRuntime::RunLoop loop;

        bool flat_map = false;
        bool map1     = false;
        bool map2     = false;

        NFuture::Just() | NFuture::Via(loop) | NFuture::FlatMap([&](Unit) {
            flat_map = true;
            return NFuture::Value(Unit());
        }) | NFuture::Map([&](Unit u) {
            map1 = true;
            return u;
        }) | NFuture::Map([&](Unit u) {
            map2 = true;
            return u;
        }) | NFuture::Detach();

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
        NRuntime::RunLoop loop;

        NFuture::Submit(loop, [&] { return NFuture::Submit(loop, [] { return 19; }); }) | NFuture::Flatten() | NFuture::Detach();

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, 2);
    }

    SIMPLE_TEST(Inline5) {
        NRuntime::RunLoop loop;

        auto [f1, p1] = NFuture::Contract<Result<int>>();
        auto [f2, p2] = NFuture::Contract<Result<int>>();

        auto first = NFuture::FirstOk(std::move(f1) | NFuture::Via(loop), std::move(f2) | NFuture::Via(loop));

        bool ok = false;

        std::move(first) | NFuture::MapOk([&ok](int v) {
            ASSERT_EQ(v, 31);
            ok = true;
            return Unit();
        }) | NFuture::Detach();

        std::move(p2).Set(NResult::Failure(IoError()));
        std::move(p1).Set(NResult::Ok(31));

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, 0);

        ASSERT_TRUE(ok);
    }
}
