
#include <DummyConcurrency/Future/Combinators.hpp>
#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Make.hpp>
#include <DummyConcurrency/Future/Terminate.hpp>
#include <DummyConcurrency/Runtime/Scheduler/RunLoop.hpp>
#include <DummyConcurrency/Runtime/Scheduler/ThreadPool.hpp>

#include <twist/ed/std/thread.hpp>

#include <wheels/test/framework.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <wheels/core/defer.hpp>

#include <fmt/core.h>

#include <atomic>
#include <chrono>

using namespace DummyConcurrency;
using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

std::error_code TimeoutError() {
    return std::make_error_code(std::errc::timed_out);
}

//////////////////////////////////////////////////////////////////////

void StressTestContract() {
    Testing::TimeBudget budget;
    int                 iter = 0;

    while (budget) {
        auto [f, p] = Future::Contract<int>();

        twist::ed::std::thread producer([p = std::move(p), iter]() mutable { std::move(p).Set((int)iter); });

        twist::ed::std::thread consumer([f = std::move(f), iter]() mutable {
            int v = Future::Get(std::move(f));
            ASSERT_EQ(v, iter);
        });

        producer.join();
        consumer.join();

        ++iter;
    }

    fmt::println("Iterations: {}", iter);
}

//////////////////////////////////////////////////////////////////////

void StressTestPipeline() {
    Runtime::ThreadPool pool{4};
    pool.Start();

    size_t iter = 0;

    Testing::TimeBudget budget;

    while (budget) {
        size_t pipelines = 1 + (iter++) % 3;

        std::atomic_size_t counter1 = 0;
        std::atomic_size_t counter2 = 0;
        std::atomic_size_t counter3 = 0;

        std::vector<Future::Future<Unit>> futs;

        for (size_t j = 0; j < pipelines; ++j) {
            auto f = Future::Submit(pool,
                                    [&]() {
                                        ++counter1;
                                        return Unit();
                                    }) |
                     Future::Via(pool) | Future::Map([&](Unit) {
                         ++counter2;
                         return Unit();
                     }) |
                     Future::Map([&](Unit) {
                         ++counter3;
                         return Unit();
                     });

            futs.push_back(std::move(f));
        }

        for (auto&& f : futs) {
            Future::Get(std::move(f));
        }

        ASSERT_EQ(counter1.load(), pipelines);
        ASSERT_EQ(counter2.load(), pipelines);
        ASSERT_EQ(counter3.load(), pipelines);

        ++iter;
    }

    fmt::println("Iterations: {}", iter);

    pool.Stop();
}

//////////////////////////////////////////////////////////////////////

void StressTestFirst() {
    Runtime::ThreadPool pool{4};
    pool.Start();

    size_t iter = 0;

    Testing::TimeBudget budget;

    while (budget) {
        size_t i = iter;

        std::atomic<size_t> done{0};

        auto f = Future::Submit(pool, [&, i] -> Result<int> {
            wheels::Defer defer([&done] { ++done; });

            if (i % 3 == 0) {
                return ResultError(TimeoutError());
            } else {
                return ResultOk(1);
            }
        });

        auto g = Future::Submit(pool, [&, i] -> Result<int> {
            wheels::Defer defer([&done] { ++done; });

            if (i % 4 == 0) {
                return ResultError(TimeoutError());
            } else {
                return ResultOk(2);
            }
        });

        auto first = Future::FirstOk(std::move(f), std::move(g));

        auto r = Future::Get(std::move(first));

        if (i % 12 != 0) {
            ASSERT_TRUE(r);
            ASSERT_TRUE((*r == 1) || (*r == 2));
        } else {
            ASSERT_FALSE(r);
        }

        // Barrier
        while (done.load() != 2) {
            twist::ed::std::this_thread::yield();
        }

        ++iter;
    }

    fmt::println("Iterations: {}", iter);

    pool.Stop();
}

//////////////////////////////////////////////////////////////////////

void StressTestBoth() {
    Runtime::ThreadPool pool{4};
    pool.Start();

    size_t iter = 0;

    Testing::TimeBudget budget;

    while (budget) {
        size_t i = iter;

        auto f = Future::Submit(pool, [i] -> Result<int> {
            if (i % 7 == 5) {
                return ResultError(TimeoutError());
            } else {
                return ResultOk(1);
            }
        });

        auto g = Future::Submit(pool, [i]() -> Result<int> {
            if (i % 7 == 6) {
                return ResultError(TimeoutError());
            } else {
                return ResultOk(2);
            }
        });

        auto both = Future::BothOk(std::move(f), std::move(g));

        auto r = Future::Get(std::move(both));

        if (i % 7 < 5) {
            auto [x, y] = *r;
            ASSERT_EQ(x, 1);
            ASSERT_EQ(y, 2);
        } else {
            ASSERT_FALSE(r);
        }

        ++iter;
    }

    fmt::println("Iterations: {}", iter);

    pool.Stop();
}

//////////////////////////////////////////////////////////////////////

TEST_SUITE(StressFutures) {
    TWIST_STRESS_TEST(Contract, 5s) {
        StressTestContract();
    }

    TWIST_STRESS_TEST(Pipeline, 5s) {
        StressTestPipeline();
    }

    TWIST_STRESS_TEST(FirstOk, 5s) {
        StressTestFirst();
    }

    TWIST_STRESS_TEST(BothOk, 5s) {
        StressTestBoth();
    }
}
