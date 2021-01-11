#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/test/assert.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(SchedulingThreadPool_Stress) {
    TWIST_STRESS_TEST(SubmitAndWait, 5s) {
        ThreadPool pool{4};
        pool.Start();

        Testing::TimeBudget time_budget;

        for (size_t iter = 0; time_budget; ++iter) {
            size_t                        todo = 1 + iter % 11;
            twist::ed::std::atomic_size_t done{0};

            TWaitGroup wg;
            for (size_t i = 0; i < todo; ++i) {
                wg.Add(1);
                Submit(pool, [&] {
                    ++done;
                    wg.Done();
                });
            }

            wg.Wait();

            TWIST_TEST_ASSERT(done.load() == todo, "Missing tasks");
        }

        pool.Stop();
    }
}
