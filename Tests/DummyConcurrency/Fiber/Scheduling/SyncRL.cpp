#include <DummyConcurrency/DummyConcurrency.hpp>

#include <twist/sim.hpp>
#include <twist/test/assert.hpp>
#include <wheels/test/framework.hpp>

static_assert(twist::build::IsolatedSim());

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(FiberScheduling_SyncRL) {
    SIMPLE_TEST(NoSynchronization) {
        twist::sim::sched::RandomScheduler scheduler;
        twist::sim::Simulator              simulator{&scheduler};

        auto result = simulator.Run([] {
            RunLoop loop;

            for (size_t i = 0; i < 11; ++i) {
                Go(loop, [] {
                    for (size_t i = 0; i < 27; ++i) {
                        Yield();
                    }
                });
            }

            loop.Run();

            TWIST_TEST_ASSERT(twist::sim::stat::AtomicCount() <= 3, "Synchronization detected");
        });

        ASSERT_TRUE_M(result.Ok(), result.std_err);
    }
}