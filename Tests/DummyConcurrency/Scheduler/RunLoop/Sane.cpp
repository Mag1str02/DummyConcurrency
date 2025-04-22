#include <DummyConcurrency/DummyConcurrency.hpp>

#include <twist/sim.hpp>
#include <twist/test/assert.hpp>
#include <wheels/test/framework.hpp>

static_assert(twist::build::IsolatedSim());

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(SchedulingRunLoop_Sane) {
    SIMPLE_TEST(NoSynchronization) {
        twist::sim::sched::CoopScheduler scheduler;
        twist::sim::Simulator            simulator{&scheduler};

        auto result = simulator.Run([] {
            RunLoop loop;

            Submit(loop, [] {});
            TWIST_TEST_ASSERT(loop.Run() == 1, "Missing task");

            size_t atomic_count = twist::sim::stat::AtomicCount();
            TWIST_TEST_ASSERT(atomic_count == 0, "Unexpected synchronization");
        });

        ASSERT_TRUE_M(result.Ok(), result.std_err);
    }
}
