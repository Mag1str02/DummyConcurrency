#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_Unit) {
    SIMPLE_TEST(Lock) {
        RunLoop scheduler;

        FMutex mutex;
        size_t cs = 0;

        Go(scheduler, [&] {
            mutex.Lock();
            ++cs;
            mutex.Unlock();

            mutex.Lock();
            ++cs;
            mutex.Unlock();
        });

        scheduler.Run();

        ASSERT_EQ(cs, 2);
    }

    SIMPLE_TEST(TryLock) {
        RunLoop scheduler;

        Go(scheduler, [&] {
            FMutex mutex;

            {
                ASSERT_TRUE(mutex.TryLock());
                mutex.Unlock();
            }

            {
                mutex.Lock();
                mutex.Unlock();
            }

            ASSERT_TRUE(mutex.TryLock());

            bool join = false;

            Go([&] {
                ASSERT_FALSE(mutex.TryLock());
                join = true;
            });

            while (!join) {
                Yield();
            }

            mutex.Unlock();
        });

        scheduler.Run();
    }

    SIMPLE_TEST(Lockable) {
        RunLoop scheduler;

        Go(scheduler, [&] {
            FMutex mutex;

            { std::lock_guard guard{mutex}; }

            {
                std::unique_lock lock{mutex, std::try_to_lock};
                ASSERT_TRUE(lock.owns_lock());
            }
        });

        scheduler.Run();
    }

    SIMPLE_TEST(LockManyTimes) {
        RunLoop scheduler;

        FMutex mutex;
        size_t cs = 0;

        Go(scheduler, [&] {
            for (size_t j = 0; j < 11; ++j) {
                std::lock_guard guard(mutex);
                ++cs;
            }
        });

        scheduler.Run();

        ASSERT_EQ(cs, 11);
    }

    SIMPLE_TEST(Counter) {
        RunLoop scheduler;

        FMutex mutex;
        size_t cs = 0;

        static const size_t kFibers           = 5;
        static const size_t kSectionsPerFiber = 5;

        for (size_t i = 0; i < kFibers; ++i) {
            Go(scheduler, [&] {
                for (size_t j = 0; j < kSectionsPerFiber; ++j) {
                    std::lock_guard guard(mutex);

                    ++cs;
                    Yield();
                }
            });
        }

        scheduler.Run();

        ASSERT_EQ(cs, kFibers * kSectionsPerFiber);
    }

    SIMPLE_TEST(SuspendFiber) {
        RunLoop scheduler;

        FMutex mutex;
        FEvent unlock;

        Go(scheduler, [&] {
            mutex.Lock();
            unlock.Wait();
            mutex.Unlock();
        });

        bool cs = false;

        Go(scheduler, [&] {
            mutex.Lock();
            cs = true;
            mutex.Unlock();
        });

        {
            size_t tasks = scheduler.Run();
            ASSERT_LE(tasks, 17);
            ASSERT_FALSE(cs);
        }

        Go(scheduler, [&] { unlock.Fire(); });

        scheduler.Run();

        ASSERT_TRUE(cs);
    }

    SIMPLE_TEST(Fifo) {
        RunLoop scheduler;

        FMutex mutex;

        Go(scheduler, [&] {
            mutex.Lock();

            for (size_t i = 0; i < 1024; ++i) {
                Yield();
            }

            mutex.Unlock();
        });

        static const size_t kWaiters = 16;

        scheduler.RunAtMost(7);  // Lock mutex

        size_t next_waiter = 0;

        for (size_t i = 0; i < kWaiters; ++i) {
            Go(scheduler, [&, i] {
                mutex.Lock();

                ASSERT_EQ(next_waiter, i);
                ++next_waiter;

                mutex.Unlock();
            });

            // mutex.Lock() -> wait queue
            scheduler.RunAtMost(7);
        }

        scheduler.Run();
    }
}
