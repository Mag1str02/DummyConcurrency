#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationEvent_Unit) {
    SIMPLE_TEST(OneWaiter) {
        RunLoop scheduler;

        static const std::string kMessage = "Hello";

        FEvent      event;
        std::string data;
        bool        ok = false;

        Go(scheduler, [&] {
            event.Wait();
            ASSERT_EQ(data, kMessage);
            ok = true;
        });

        Go(scheduler, [&] {
            data = kMessage;
            event.Fire();
        });

        scheduler.Run();

        ASSERT_TRUE(ok);
    }

    SIMPLE_TEST(MultipleWaiters) {
        RunLoop scheduler;

        FEvent event;
        int    work    = 0;
        size_t waiters = 0;

        static const size_t kWaiters = 7;

        for (size_t i = 0; i < kWaiters; ++i) {
            Go(scheduler, [&] {
                event.Wait();
                ASSERT_EQ(work, 1);
                ++waiters;
            });
        }

        Go(scheduler, [&] {
            ++work;
            event.Fire();
        });

        scheduler.Run();

        ASSERT_EQ(waiters, kWaiters);
    }

    SIMPLE_TEST(SuspendFiber) {
        RunLoop scheduler;

        FEvent event;
        bool   ok = false;

        Go(scheduler, [&] {
            event.Wait();
            ok = true;
        });

        {
            size_t tasks = scheduler.Run();
            ASSERT_LE(tasks, 7);
        }

        Go(scheduler, [&] { event.Fire(); });

        scheduler.Run();

        ASSERT_TRUE(ok);
    }
}
