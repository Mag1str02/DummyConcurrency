#include <DummyConcurrency/Syntax/Syntax.hpp>
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

using namespace NDummyConcurrency;
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
        auto [f, p] = NFuture::Contract<int>();

        twist::ed::std::thread producer([p = std::move(p), iter]() mutable { std::move(p).Set((int)iter); });

        twist::ed::std::thread consumer([f = std::move(f), iter]() mutable {
            int v = NFuture::Get(std::move(f));
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
    NRuntime::ThreadPool pool{4};
    pool.Start();

    size_t iter = 0;

    Testing::TimeBudget budget;

    while (budget) {
        size_t pipelines = 1 + (iter++) % 3;

        std::atomic_size_t counter1 = 0;
        std::atomic_size_t counter2 = 0;
        std::atomic_size_t counter3 = 0;

        std::vector<Future<Unit>> futs;

        for (size_t j = 0; j < pipelines; ++j) {
            auto f = NFuture::Submit(pool,
                                    [&]() {
                                        ++counter1;
                                        return Unit();
                                    }) |
                     NFuture::Via(pool) | NFuture::Map([&](Unit) {
                         ++counter2;
                         return Unit();
                     }) |
                     NFuture::Map([&](Unit) {
                         ++counter3;
                         return Unit();
                     });

            futs.push_back(std::move(f));
        }

        for (auto&& f : futs) {
            NFuture::Get(std::move(f));
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
    NRuntime::ThreadPool pool{4};
    pool.Start();

    size_t iter = 0;

    Testing::TimeBudget budget;

    while (budget) {
        size_t i = iter;

        std::atomic<size_t> done{0};

        auto f = NFuture::Submit(pool, [&, i] -> Result<int> {
            wheels::Defer defer([&done] { ++done; });

            if (i % 3 == 0) {
                return NResult::Failure(TimeoutError());
            } else {
                return NResult::Ok(1);
            }
        });

        auto g = NFuture::Submit(pool, [&, i] -> Result<int> {
            wheels::Defer defer([&done] { ++done; });

            if (i % 4 == 0) {
                return NResult::Failure(TimeoutError());
            } else {
                return NResult::Ok(2);
            }
        });

        auto first = NFuture::FirstOk(std::move(f), std::move(g));

        auto r = NFuture::Get(std::move(first));

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
    NRuntime::ThreadPool pool{4};
    pool.Start();

    size_t iter = 0;

    Testing::TimeBudget budget;

    while (budget) {
        size_t i = iter;

        auto f = NFuture::Submit(pool, [i] -> Result<int> {
            if (i % 7 == 5) {
                return NResult::Failure(TimeoutError());
            } else {
                return NResult::Ok(1);
            }
        });

        auto g = NFuture::Submit(pool, [i]() -> Result<int> {
            if (i % 7 == 6) {
                return NResult::Failure(TimeoutError());
            } else {
                return NResult::Ok(2);
            }
        });

        auto both = NFuture::BothOk(std::move(f), std::move(g));

        auto r = NFuture::Get(std::move(both));

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
