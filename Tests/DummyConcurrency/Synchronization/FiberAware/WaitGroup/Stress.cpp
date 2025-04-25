#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <twist/test/assert.hpp>

#include <fmt/core.h>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationWaitGroup_Stress) {
    TWIST_STRESS_TEST(Load, 5s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        TWaitGroup test;

        test.Add(1);

        Go(scheduler, [&] {
            Testing::TimeBudget time_budget;

            FWaitGroup wg;

            size_t iter_count = 0;

            while (time_budget) {
                ++iter_count;

                size_t fibers = 3 + iter_count % 5;

                twist::ed::std::atomic_size_t work = 0;

                for (size_t i = 0; i < fibers; ++i) {
                    wg.Add(1);

                    Go([&] {
                        for (size_t j = 0; j < 7; ++j) {
                            NFiber::Yield();
                        }
                        work.fetch_add(1);
                        wg.Done();
                    });
                }

                wg.Wait();

                TWIST_TEST_ASSERT(work.load() == fibers, "Missing work");
            }

            fmt::println("# groups = {}", iter_count);

            test.Done();
        });

        test.Wait();

        scheduler.Stop();
    }
}
