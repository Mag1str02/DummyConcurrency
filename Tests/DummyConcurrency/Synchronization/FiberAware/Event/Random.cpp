#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

#include <twist/assist/shared.hpp>
#include <twist/test/assert.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationEvent_Random) {
    TWIST_RANDOMIZE(MessagePassing, 10s) {
        ThreadPool scheduler{3};
        scheduler.Start();

        {
            TWaitGroup test;

            twist::assist::Shared<int> data = 0;
            FEvent                     event;

            test.Add(1);

            Go(scheduler, [&] {
                data.Write(1);
                event.Fire();

                test.Done();
            });

            test.Add(1);

            Go(scheduler, [&] {
                event.Wait();

                int d = data.Read();
                TWIST_TEST_ASSERT(d == 1, "Unfinished work");

                test.Done();
            });

            test.Add(1);

            Runtime::Submit(scheduler, [&] {
                event.Wait();

                int d = data.Read();
                TWIST_TEST_ASSERT(d == 1, "Unfinished work");

                test.Done();
            });

            test.Wait();
        }

        scheduler.Stop();
    }
}
