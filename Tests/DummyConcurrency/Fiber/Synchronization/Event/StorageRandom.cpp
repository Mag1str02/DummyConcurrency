#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationEvent_StorageRandom) {
    TWIST_RANDOMIZE(Storage, 5s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            test.Add(1);

            Go(scheduler, [&test] {
                auto* event = new FEvent{};

                Go([event] { event->Fire(); });

                event->Wait();
                delete event;

                test.Done();
            });

            test.Wait();
        }

        scheduler.Stop();
    }
}
