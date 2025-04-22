
#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

#include <twist/ed/std/thread.hpp>
#include <twist/mod/sim/stat/memory.hpp>
#include <twist/test/assert.hpp>
#include <twist/test/plate.hpp>
#include <wheels/test/framework.hpp>

#include <fmt/core.h>

static_assert(twist::build::IsolatedSim());

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_Alloc) {
    TWIST_RANDOMIZE(NoAllocations, 10s) {
        ThreadPool pool{3};
        pool.Start();

        TWaitGroup wg;

        constexpr size_t kFibers = 5;
        constexpr size_t kIters  = 11;

        twist::test::Plate plate;
        FMutex             mutex;

        for (size_t i = 0; i < kFibers; ++i) {
            wg.Add(1);

            Go(pool, [&] {
                twist::ed::std::this_thread::yield();

                for (size_t j = 0; j < kIters; ++j) {
                    mutex.Lock();
                    plate.Access();
                    mutex.Unlock();
                }

                wg.Done();
            });
        }

        size_t alloc_count1 = twist::sim::stat::AllocationCount();

        wg.Wait();

        TWIST_TEST_ASSERT(plate.AccessCount() == kFibers * kIters, "Missing critical sections");

        size_t alloc_count2 = twist::sim::stat::AllocationCount();

        TWIST_TEST_ASSERT(alloc_count1 == alloc_count2, "Dynamic memory allocations");

        pool.Stop();
    }
}
