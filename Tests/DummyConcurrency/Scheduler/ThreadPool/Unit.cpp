#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/core/stop_watch.hpp>
#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <chrono>  // IWYU pragma: keep
#include <thread>

using namespace std::chrono_literals;  // NOLINT
using namespace NDummyConcurrency;      // NOLINT

TEST_SUITE(SchedulingThreadPool_Unit) {
    SIMPLE_TEST(WaitTask) {
        ThreadPool pool{4};

        pool.Start();

        TWaitGroup wg;

        wg.Add(1);
        Submit(pool, [&wg] { wg.Done(); });

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(Wait) {
        ThreadPool pool{4};

        pool.Start();

        TWaitGroup wg;

        wg.Add(1);
        Submit(pool, [&wg] {
            std::this_thread::sleep_for(1s);
            wg.Done();
        });

        wg.Wait();
        pool.Stop();
    }

    SIMPLE_TEST(MultiWait) {
        ThreadPool pool{1};

        pool.Start();

        for (size_t i = 0; i < 3; ++i) {
            TWaitGroup wg;

            wg.Add(1);

            Submit(pool, [&wg] {
                std::this_thread::sleep_for(1s);
                wg.Done();
            });

            wg.Wait();
        }

        pool.Stop();
    }

    SIMPLE_TEST(ManyTasks) {
        ThreadPool pool{4};

        pool.Start();

        static const size_t kTasks = 17;

        TWaitGroup wg;

        for (size_t i = 0; i < kTasks; ++i) {
            wg.Add(1);
            Submit(pool, [&wg] { wg.Done(); });
        }

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(Parallel) {
        ThreadPool pool{4};

        pool.Start();

        std::atomic<bool> fast{false};

        TWaitGroup wg;

        wg.Add(1);
        Submit(pool, [&] {
            std::this_thread::sleep_for(1s);
            wg.Done();
        });

        wg.Add(1);
        Submit(pool, [&] {
            fast.store(true);
            wg.Done();
        });

        std::this_thread::sleep_for(100ms);

        ASSERT_EQ(fast.load(), true);

        wg.Wait();
        pool.Stop();
    }

    SIMPLE_TEST(TwoPools) {
        ThreadPool pool1{1};
        ThreadPool pool2{1};

        pool1.Start();
        pool2.Start();

        wheels::StopWatch stop_watch;

        TWaitGroup wg1;
        wg1.Add(1);
        Submit(pool1, [&] {
            std::this_thread::sleep_for(1s);
            wg1.Done();
        });

        TWaitGroup wg2;
        wg2.Add(1);
        Submit(pool2, [&] {
            std::this_thread::sleep_for(1s);
            wg2.Done();
        });

        wg2.Wait();
        pool2.Stop();

        wg1.Wait();
        pool1.Stop();

        ASSERT_TRUE(stop_watch.Elapsed() < 1500ms);
    }

    SIMPLE_TEST(DoNotBurnCPU) {
        ThreadPool pool{4};

        pool.Start();

        TWaitGroup wg;

        // Warmup
        for (size_t i = 0; i < 4; ++i) {
            wg.Add(1);
            Submit(pool, [&wg] {
                std::this_thread::sleep_for(100ms);
                wg.Done();
            });
        }

        wheels::ProcessCPUTimer cpu_timer;

        std::this_thread::sleep_for(1s);

        wg.Wait();
        pool.Stop();

        ASSERT_TRUE(cpu_timer.Spent() < 100ms);
    }

    SIMPLE_TEST(Current) {
        ThreadPool pool{1};
        pool.Start();

        ASSERT_EQ(ThreadPool::Current(), nullptr);

        TWaitGroup wg;
        wg.Add(1);

        Submit(pool, [&] {
            ASSERT_EQ(ThreadPool::Current(), &pool);
            wg.Done();
        });

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(SubmitAfterWait) {
        ThreadPool pool{4};

        pool.Start();

        TWaitGroup wg;

        wg.Add(1);
        Submit(pool, [&] {
            std::this_thread::sleep_for(500ms);

            wg.Add(1);
            Submit(pool, [&] {
                std::this_thread::sleep_for(500ms);
                wg.Done();
            });

            wg.Done();
        });

        wg.Wait();

        pool.Stop();
    }

    TEST(UseThreads, wheels::test::TestOptions().TimeLimit(1s)) {
        ThreadPool pool{4};
        pool.Start();

        TWaitGroup wg;

        for (size_t i = 0; i < 4; ++i) {
            wg.Add(1);
            Submit(pool, [&wg] {
                std::this_thread::sleep_for(750ms);
                wg.Done();
            });
        }

        wg.Wait();
        pool.Stop();
    }

    TEST(TooManyThreads, wheels::test::TestOptions().TimeLimit(2s)) {
        ThreadPool pool{3};

        pool.Start();

        TWaitGroup wg;

        for (size_t i = 0; i < 4; ++i) {
            wg.Add(1);
            Submit(pool, [&wg] {
                std::this_thread::sleep_for(750ms);
                wg.Done();
            });
        }

        wheels::StopWatch stop_watch;

        wg.Wait();
        pool.Stop();

        ASSERT_TRUE(stop_watch.Elapsed() > 1s);
    }

    SIMPLE_TEST(TaskLifetime) {
        ThreadPool pool{4};

        pool.Start();

        struct Widget {};

        auto w = std::make_shared<Widget>();

        TWaitGroup wg;
        for (int i = 0; i < 4; ++i) {
            wg.Add(1);
            Submit(pool, [w, &wg] { wg.Done(); });
        }

        std::this_thread::sleep_for(500ms);

        ASSERT_EQ(w.use_count(), 1)

        wg.Wait();

        pool.Stop();
    }

    SIMPLE_TEST(Racy) {
        ThreadPool pool{4};

        pool.Start();

        std::atomic<size_t> racy_counter{0};

        static const size_t kTasks = 100500;

        TWaitGroup wg;

        for (size_t i = 0; i < kTasks; ++i) {
            wg.Add(1);
            Submit(pool, [&] {
                int old = racy_counter.load();
                racy_counter.store(old + 1);

                wg.Done();
            });
        }

        wg.Wait();

        pool.Stop();

        std::cout << "Racy counter value: " << racy_counter << std::endl;

        ASSERT_LE(racy_counter.load(), kTasks);
    }
}
