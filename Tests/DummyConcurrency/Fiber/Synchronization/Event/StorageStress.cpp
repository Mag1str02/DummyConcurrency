#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <fmt/core.h>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationEvent_StorageStress) {
    TWIST_STRESS_TEST(Storage, 5s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        Testing::TimeBudget time_budget;

        size_t iter_count = 0;

        while (time_budget) {
            ++iter_count;

            TWaitGroup iter;

            iter.Add(1);

            Go(scheduler, [&iter] {
                auto* event = new FEvent{};

                Go([event] { event->Fire(); });

                event->Wait();
                delete event;

                iter.Done();
            });

            iter.Wait();
        }

        scheduler.Stop();

        fmt::println("# iterations = {}", iter_count);
    }
}
