#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

#include <twist/assist/shared.hpp>
#include <twist/test/assert.hpp>

#include <array>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationWaitGroup_Random) {
    TWIST_RANDOMIZE(OneWaiter, 5s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            const size_t kFibers = 4;

            std::array<twist::assist::Shared<bool>, kFibers> flags = {false};

            test.Add(1);

            Go(scheduler, [&] {
                FWaitGroup wg;

                twist::ed::std::atomic_size_t left{kFibers};

                for (size_t i = 0; i < kFibers; ++i) {
                    wg.Add(1);

                    Go([&, i] {
                        for (size_t j = 0; j < 3; ++j) {
                            Yield();
                        }

                        flags[i].Write(true);
                        left.fetch_sub(1);
                        wg.Done();
                    });
                }

                wg.Wait();

                TWIST_TEST_ASSERT(left.load() == 0, "Unfinished fibers");

                for (size_t i = 0; i < kFibers; ++i) {
                    bool f = flags[i].Read();
                    TWIST_TEST_ASSERT(f, "Missing work");
                }

                test.Done();
            });

            test.Wait();
        }

        scheduler.Stop();
    }

    TWIST_RANDOMIZE(Waiters, 10s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            const size_t kWorkers = 3;
            const size_t kWaiters = 3;

            std::array<twist::assist::Shared<bool>, kWorkers> flags = {false};

            test.Add(1);

            Go(scheduler, [&] {
                FWaitGroup work;
                FWaitGroup wait;

                for (size_t i = 0; i < kWorkers; ++i) {
                    work.Add(1);

                    Go([&, i] {
                        for (size_t k = 0; k < 3; ++k) {
                            Yield();
                        }

                        flags[i].Write(true);
                        work.Done();
                    });
                }

                for (size_t j = 0; j < kWaiters; ++j) {
                    wait.Add(1);

                    Go([&] {
                        for (size_t k = 0; k < 3; ++k) {
                            Yield();
                        }

                        work.Wait();

                        for (size_t i = 0; i < kWorkers; ++i) {
                            bool f = flags[i].Read();
                            TWIST_TEST_ASSERT(f, "Missing work");
                        }

                        wait.Done();
                    });
                }

                wait.Wait();
                test.Done();
            });

            test.Wait();
        }

        scheduler.Stop();
    }

    TWIST_RANDOMIZE(Cyclic, 10s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        const size_t kIters = 3;

        {
            TWaitGroup test;

            const size_t kWorkers = 3;

            std::array<twist::assist::Shared<bool>, kWorkers> flags = {false};

            test.Add(1);

            Go(scheduler, [&] {
                FWaitGroup work;

                for (size_t k = 0; k < kIters; ++k) {
                    // New epoch

                    for (size_t i = 0; i < kWorkers; ++i) {
                        work.Add(1);

                        Go([&, i] {
                            flags[i].Write(true);
                            work.Done();
                        });
                    }

                    work.Wait();

                    for (size_t i = 0; i < kWorkers; ++i) {
                        bool f = flags[i].Read();
                        TWIST_TEST_ASSERT(f, "Missing work");
                    }
                }

                test.Done();
            });

            test.Wait();
        }

        scheduler.Stop();
    }
}
