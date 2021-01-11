#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/core/stop_watch.hpp>
#include <wheels/test/framework.hpp>

#include <chrono>  // IWYU pragma: keep
#include <thread>

using namespace std::chrono_literals;  // NOLINT
using namespace DummyConcurrency;      // NOLINT

void ExpectScheduler(ThreadPool& pool) {
    ASSERT_EQ(ThreadPool::Current(), &pool);
}

TEST_SUITE(FiberScheduling_UnitTP) {
    SIMPLE_TEST(Go) {
        ThreadPool pool{3};
        pool.Start();

        TWaitGroup wg;

        wg.Add(1);
        Go(pool, [&]() {
            ExpectScheduler(pool);
            wg.Done();
        });

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(GoGroup) {
        ThreadPool pool{4};
        pool.Start();

        TWaitGroup wg;

        const size_t kFibers = 7;

        for (size_t i = 0; i < kFibers; ++i) {
            wg.Add(1);
            Go(pool, [&] {
                ExpectScheduler(pool);
                wg.Done();
            });
        }

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(GoChild) {
        ThreadPool pool{3};
        pool.Start();

        TWaitGroup wg;

        wg.Add(1);

        Go(pool, [&] {
            ExpectScheduler(pool);

            wg.Add(1);
            Go([&] {
                ExpectScheduler(pool);
                wg.Done();
            });

            wg.Done();
        });

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(Parallel) {
        const size_t kThreads = 4;

        ThreadPool pool{kThreads};
        pool.Start();

        wheels::StopWatch stop_watch;

        TWaitGroup wg;

        for (size_t i = 0; i < kThreads; ++i) {
            wg.Add(1);
            Go(pool, [&wg] {
                std::this_thread::sleep_for(1s);
                wg.Done();
            });
        }

        wg.Wait();

        ASSERT_TRUE(stop_watch.Elapsed() < 2s);

        pool.Stop();
    }

    SIMPLE_TEST(Yield) {
        ThreadPool pool{1};
        pool.Start();

        TWaitGroup wg;

        wg.Add(1);
        Go(pool, [&] {
            Yield();

            ExpectScheduler(pool);

            wg.Done();
        });

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(YieldChild) {
        ThreadPool loop{1};
        loop.Start();

        TWaitGroup wg;

        wg.Add(1);
        Go(loop, [&wg] {
            bool child = false;

            Go([&] { child = true; });

            while (!child) {
                Yield();
            }

            wg.Done();
        });

        wg.Wait();

        loop.Stop();
    }

    SIMPLE_TEST(ForYield) {
        ThreadPool loop{1};
        loop.Start();

        const size_t kYields = 128;

        size_t yields = 0;

        TWaitGroup wg;

        wg.Add(1);

        Go(loop, [&] {
            for (size_t i = 0; i < kYields; ++i) {
                Yield();
                ++yields;
            }

            wg.Done();
        });

        wg.Wait();

        ASSERT_EQ(yields, kYields);

        loop.Stop();
    }

    SIMPLE_TEST(PingPong) {
        ThreadPool loop{1};
        loop.Start();

        bool start = false;
        int  turn  = 0;

        TWaitGroup wg;

        wg.Add(2);

        const size_t kRounds = 3;

        Go(loop, [&] {
            while (!start) {
                Yield();
            }

            for (size_t i = 0; i < kRounds; ++i) {
                ASSERT_TRUE(turn == 0)
                turn ^= 1;

                Yield();
            }

            wg.Done();
        });

        Go(loop, [&] {
            {
                start = true;
                Yield();
            }

            for (size_t j = 0; j < kRounds; ++j) {
                ASSERT_TRUE(turn == 1);
                turn ^= 1;

                Yield();
            }

            wg.Done();
        });

        wg.Wait();

        loop.Stop();
    }

    SIMPLE_TEST(YieldGroup) {
        ThreadPool pool{4};
        pool.Start();

        const size_t kFibers = 5;
        const size_t kYields = 7;

        TWaitGroup wg;

        for (size_t i = 0; i < kFibers; ++i) {
            wg.Add(1);

            Go(pool, [&] {
                for (size_t j = 0; j < kYields; ++j) {
                    Yield();
                }
                wg.Done();
            });
        }

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(TwoPools) {
        ThreadPool pool1{3};
        pool1.Start();

        ThreadPool pool2{3};
        pool2.Start();

        TWaitGroup wg;

        wg.Add(2);

        Go(pool1, [&] {
            for (size_t i = 0; i < 2; ++i) {
                Yield();
                ExpectScheduler(pool1);
            }
            wg.Done();
        });

        Go(pool2, [&] {
            for (size_t j = 0; j < 3; ++j) {
                Yield();
                ExpectScheduler(pool2);
            }
            wg.Done();
        });

        wg.Wait();

        pool1.Stop();
        pool2.Stop();
    }
}
