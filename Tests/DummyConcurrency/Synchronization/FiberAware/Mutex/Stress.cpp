#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <twist/assist/random.hpp>
#include <twist/test/plate.hpp>

#include <fmt/core.h>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_Stress) {
    TWIST_STRESS_TEST(Iter, 5s) {
        twist::ed::std::random_device rd{};
        twist::assist::Choice         choice{rd};

        ThreadPool scheduler{3};
        scheduler.Start();

        Testing::TimeBudget time_budget;

        size_t iter_count = 0;

        while (time_budget) {
            ++iter_count;

            // Parameters
            size_t fibers = choice(1, 6);
            size_t writes = choice(1, 5);

            // Test control
            TWaitGroup iter;

            // Data
            twist::test::Plate plate;
            FMutex             mutex;

            for (size_t i = 0; i < fibers; ++i) {
                iter.Add(1);

                Go(scheduler, [&] {
                    for (size_t j = 0; j < writes; ++j) {
                        mutex.Lock();
                        plate.Access();
                        mutex.Unlock();
                    }

                    iter.Done();
                });
            }

            iter.Wait();
        }

        scheduler.Stop();

        fmt::println("# iterations = {}", iter_count);
    }

    TWIST_STRESS_TEST(Load, 5s) {
        // Parameters
        const size_t kFibers  = 11;
        const size_t kThreads = 3;

        ThreadPool scheduler{kThreads};
        scheduler.Start();

        TWaitGroup wg;

        FMutex             mutex;
        twist::test::Plate plate;

        for (size_t i = 0; i < kFibers; ++i) {
            wg.Add(1);

            Go(scheduler, [&] {
                Testing::TimeBudget time_budget;

                bool lock = false;

                while (time_budget) {
                    lock = !lock;

                    if (lock) {
                        mutex.Lock();
                        plate.Access();
                        mutex.Unlock();
                    } else {
                        if (mutex.TryLock()) {
                            plate.Access();
                            mutex.Unlock();
                        }
                    }
                }

                wg.Done();
            });
        }

        wg.Wait();

        scheduler.Stop();
    }
}
