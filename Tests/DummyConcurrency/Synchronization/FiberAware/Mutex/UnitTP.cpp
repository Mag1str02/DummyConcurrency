#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>

using namespace DummyConcurrency;      // NOLINT
using namespace std::chrono_literals;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_UnitTP) {
    TEST(UnlockFork, wheels::test::TestOptions().TimeLimit(10s)) {
        ThreadPool scheduler{4};
        scheduler.Start();

        FMutex mutex;

        TWaitGroup test;
        test.Add(4);

        Go(scheduler, [&] {
            mutex.Lock();

            std::this_thread::sleep_for(1s);

            mutex.Unlock();

            test.Done();
        });

        for (size_t i = 0; i < 3; ++i) {
            Go(scheduler, [&] {
                mutex.Lock();
                // Sequential
                mutex.Unlock();

                // Parallel
                std::this_thread::sleep_for(5s);

                test.Done();
            });
        }

        test.Wait();

        scheduler.Stop();
    }
}
