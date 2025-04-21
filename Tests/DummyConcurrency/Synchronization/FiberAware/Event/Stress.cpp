#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <twist/assist/shared.hpp>
#include <twist/test/assert.hpp>

#include <fmt/core.h>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationEvent_Stress) {
    TWIST_STRESS_TEST(MessagePassing, 5s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        Testing::TimeBudget time_budget;

        size_t iter_count = 0;

        while (time_budget) {
            ++iter_count;

            TWaitGroup iter;

            twist::assist::Shared<int> data = 0;
            FEvent                     event;

            iter.Add(1);

            Scheduler::Submit(scheduler, [&] {
                event.Wait();

                int d = data.Read();
                TWIST_TEST_ASSERT(d == 1, "Missing message");

                iter.Done();
            });

            iter.Add(1);

            Go(scheduler, [&] {
                data.Write(1);
                event.Fire();

                iter.Done();
            });

            iter.Add(1);

            Go(scheduler, [&] {
                event.Wait();

                int d = data.Read();
                TWIST_TEST_ASSERT(d == 1, "Missing message");

                iter.Done();
            });

            iter.Wait();
        }

        scheduler.Stop();

        fmt::println("# iterations = {}", iter_count);
    }
}
