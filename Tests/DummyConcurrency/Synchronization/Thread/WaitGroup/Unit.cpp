#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <atomic>
#include <thread>
#include <vector>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(ThreadSynchronizationWaitGroup_Unit) {
    SIMPLE_TEST(JustWorks) {
        TWaitGroup wg;

        wg.Add(1);
        wg.Done();
        wg.Wait();
    }

    SIMPLE_TEST(InitZero) {
        TWaitGroup wg;
        wg.Wait();
    }

    SIMPLE_TEST(AddCount) {
        TWaitGroup wg;

        wg.Add(7);

        for (size_t i = 0; i < 7; ++i) {
            wg.Done();
        }

        wg.Wait();
    }

    SIMPLE_TEST(Wait) {
        TWaitGroup wg;
        bool       ready = false;

        wg.Add(1);

        std::thread producer([&] {
            std::this_thread::sleep_for(1s);
            ready = true;
            wg.Done();
        });

        wg.Wait();
        ASSERT_TRUE(ready);

        producer.join();
    }

    SIMPLE_TEST(MultiWait) {
        std::vector<std::thread> threads;

        TWaitGroup wg;

        std::atomic<size_t> work{0};

        static const size_t kWorkers = 3;
        static const size_t kWaiters = 4;

        wg.Add(kWorkers);

        for (size_t i = 0; i < kWaiters; ++i) {
            threads.emplace_back([&] {
                wg.Wait();
                ASSERT_EQ(work.load(), kWorkers);
            });
        }

        for (size_t i = 1; i <= kWorkers; ++i) {
            threads.emplace_back([&, i] {
                std::this_thread::sleep_for(256ms * i);
                ++work;
                wg.Done();
            });
        }

        for (auto&& t : threads) {
            t.join();
        }
    }

    SIMPLE_TEST(BlockingWait) {
        TWaitGroup wg;

        static const size_t kWorkers = 3;

        std::vector<std::thread> workers;
        std::atomic<size_t>      work = 0;

        wg.Add(kWorkers);

        for (size_t i = 0; i < kWorkers; ++i) {
            workers.emplace_back([&] {
                std::this_thread::sleep_for(1s);
                ++work;
                wg.Done();
            });
        }

        wheels::ProcessCPUTimer timer;

        wg.Wait();

        ASSERT_TRUE(timer.Spent() < 100ms);

        for (auto& t : workers) {
            t.join();
        }
    }

    SIMPLE_TEST(Cyclic) {
        TWaitGroup wg;

        for (size_t i = 0; i < 4; ++i) {
            bool flag = false;

            wg.Add(1);

            std::thread worker([&] {
                std::this_thread::sleep_for(1s);
                flag = true;
                wg.Done();
            });

            wg.Wait();

            ASSERT_TRUE(flag);

            worker.join();
        }
    }
}
