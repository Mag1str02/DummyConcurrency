#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <twist/test/plate.hpp>

#include <fmt/core.h>

using namespace NDummyConcurrency;      // NOLINT
using namespace std::chrono_literals;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_StackOverflow) {
    TWIST_STRESS_TEST(StackOverflow, 10s) {
        // Parameters
        const size_t kFibers         = 4096;
        const size_t kThreads        = 3;
        const size_t kSmallStackSize = 8 * 1024;  // 8KB

        ThreadPool scheduler{kThreads};
        scheduler.Start();

        TWaitGroup wg;

        FMutex             mutex;
        twist::test::Plate plate;

        for (size_t i = 0; i < kFibers; ++i) {
            wg.Add(1);

            Go(scheduler,
               [&] {
                   Testing::TimeBudget time_budget{5s};

                   while (time_budget) {
                       mutex.Lock();
                       plate.Access();
                       mutex.Unlock();
                   }

                   wg.Done();
               },
               {.Pool = NFiber::InlineStackPool<kSmallStackSize>()});
        }

        wg.Wait();

        fmt::println("# cs = {}", plate.AccessCount());

        scheduler.Stop();
    }
}
