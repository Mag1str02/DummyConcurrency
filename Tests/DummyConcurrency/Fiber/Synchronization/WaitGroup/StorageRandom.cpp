#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationWaitGroup_StorageRandom) {
    TWIST_RANDOMIZE(Storage, 5s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            test.Add(1);

            Go(scheduler, [&test] {
                auto* wg = new FWaitGroup{};

                wg->Add(1);
                Go([wg] { wg->Done(); });

                wg->Wait();
                delete wg;

                test.Done();
            });

            test.Wait();
        }

        scheduler.Stop();
    }
}
