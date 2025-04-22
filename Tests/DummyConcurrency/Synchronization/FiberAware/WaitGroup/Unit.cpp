#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationWaitGroup_Unit) {
    SIMPLE_TEST(OneWaiter) {
        RunLoop scheduler;

        FWaitGroup wg;
        size_t     work = 0;
        bool       ok   = false;

        const size_t kWorkers = 3;

        for (size_t i = 0; i < kWorkers; ++i) {
            wg.Add(1);
            Go(scheduler, [&] {
                ++work;
                wg.Done();
            });
        }

        Go(scheduler, [&] {
            wg.Wait();
            ASSERT_EQ(work, kWorkers);
            ok = true;
        });

        scheduler.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(MultipleWaiters) {
        RunLoop scheduler;

        FWaitGroup wg;

        size_t work = 0;
        size_t acks = 0;

        const size_t kWorkers = 3;
        const size_t kWaiters = 4;

        for (size_t i = 0; i < kWorkers; ++i) {
            wg.Add(1);
            Go(scheduler, [&] {
                ++work;
                wg.Done();
            });
        }

        for (size_t i = 0; i < kWaiters; ++i) {
            Go(scheduler, [&] {
                wg.Wait();
                ASSERT_EQ(work, kWorkers);
                ++acks;
            });
        }

        scheduler.Run();

        ASSERT_EQ(acks, kWaiters);
    }

    SIMPLE_TEST(SuspendFiber) {
        RunLoop scheduler;

        FWaitGroup wg;
        size_t     work = 0;
        bool       ok   = false;

        const size_t kWorkers = 3;

        wg.Add(kWorkers);

        Go(scheduler, [&] {
            wg.Wait();
            ASSERT_EQ(work, kWorkers);
            ok = true;
        });

        {
            size_t tasks = scheduler.Run();
            ASSERT_LE(tasks, 7);
        }

        for (size_t i = 0; i < kWorkers; ++i) {
            Go(scheduler, [&] {
                ++work;
                wg.Done();
            });
        }

        scheduler.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(Cyclic) {
        RunLoop scheduler;

        FWaitGroup wg;

        const size_t kIters = 3;

        for (size_t k = 0; k < kIters; ++k) {
            const size_t kWork = 5;

            size_t work = 0;

            for (size_t i = 0; i < kWork; ++i) {
                wg.Add(1);
                Go(scheduler, [&] {
                    ++work;
                    wg.Done();
                });

                Go(scheduler, [&] {
                    wg.Wait();
                    ASSERT_EQ(work, kWork);
                });
            }

            scheduler.Run();
        }
    }
}
