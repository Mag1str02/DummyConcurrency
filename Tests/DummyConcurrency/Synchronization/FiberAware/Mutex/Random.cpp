#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

#include <twist/assist/random.hpp>
#include <twist/test/either.hpp>
#include <twist/test/plate.hpp>
#include <wheels/test/framework.hpp>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_Random) {
    TWIST_RANDOMIZE(Lock, 5s) {
        twist::ed::std::random_device rd{};
        twist::assist::Choice         choice{rd};

        size_t fibers = choice(1, 6);
        size_t writes = choice(1, 5);

        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            FMutex             mutex;
            twist::test::Plate plate;

            for (size_t i = 0; i < fibers; ++i) {
                test.Add(1);

                Go(scheduler, [&] {
                    for (size_t j = 0; j < writes; ++j) {
                        mutex.Lock();
                        plate.Access();
                        mutex.Unlock();
                    }

                    test.Done();
                });
            }

            test.Wait();
        }

        scheduler.Stop();
    }

    TWIST_RANDOMIZE(LockAndTryLock, 5s) {
        twist::ed::std::random_device rd{};
        twist::assist::Choice         choice{rd};

        size_t fibers = choice(1, 6);
        size_t writes = choice(1, 5);

        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            FMutex             mutex;
            twist::test::Plate plate;

            for (size_t i = 0; i < fibers; ++i) {
                test.Add(1);

                Go(scheduler, [&] {
                    for (size_t j = 0; j < writes; ++j) {
                        if (twist::test::Either()) {
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

                    test.Done();
                });
            }

            test.Wait();
        }

        scheduler.Stop();
    }
}
