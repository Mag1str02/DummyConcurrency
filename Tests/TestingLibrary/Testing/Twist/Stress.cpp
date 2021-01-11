#include "Stress.hpp"

#include <wheels/test/framework.hpp>

#if defined(__TWIST_SIM__)

// simulation

#include <twist/sim.hpp>

#if defined(__TWIST_SIM_ISOLATION__)

// matrix

#include "Testing/Twist/Simulation/Result.hpp"

namespace Testing::Twist {
    namespace Stress {

        using RandomScheduler = ::twist::sim::sched::RandomScheduler;

        struct Replay {
            ::twist::sim::sched::Schedule Schedule;
            ::twist::sim::Digest          Digest;
        };

        Replay TryRecord(TestBody body, RandomScheduler& scheduler) {
            ::twist::sim::sched::Recorder recorder{&scheduler};
            ::twist::sim::Simulator       simulator{&recorder};
            auto                          result = simulator.Run(body);
            return {recorder.GetSchedule(), result.digest};
        }

        void Test(TestBody body, Params) {
            RandomScheduler scheduler;

            ::twist::sim::Simulator simulator{&scheduler};
            auto                    result = simulator.Run(body);

            if (result.Failure()) {
                static const size_t kNotTooLarge = 256;

                if (result.iters < kNotTooLarge) {
                    auto replay = TryRecord(body, scheduler);
                    if (replay.Digest != result.digest) {
                        WHEELS_PANIC("Twist error: failed to record simulation replay");
                    }

                    ::twist::sim::Print(body, replay.Schedule);

                    FAIL_TEST(Simulation::FormatFailure(result));
                }
            } else {
                // Report
                fmt::println("Context switches injected: {}", scheduler.PreemptCount());

                return;  // Passed
            }
        }

    }  // namespace Stress
}  // namespace Testing::Twist

#else

// Simulation, maybe with sanitizers

namespace Testing::Twist {
    namespace Stress {

        void Test(TestBody body, Params) {
            ::twist::sim::sched::RandomScheduler scheduler;

            ::twist::sim::Simulator simulator{&scheduler};
            auto                    result = simulator.Run(body);

            if (result.Failure()) {
                // Impossible?
            } else {
                // Report
                fmt::println("Context switches injected: {}", scheduler.PreemptCount());

                return;  // Passed
            }
        }

    }  // namespace Stress
}  // namespace Testing::Twist

#endif

#elif defined(__TWIST_FAULTY__)

// threads + fault injection

#include <twist/rt/thr/fault/adversary/adversary.hpp>

#include <twist/rt/thr/logging/logging.hpp>

namespace Testing::Twist {
    namespace Stress {

        void Test(TestBody body, Params) {
            ::twist::rt::thr::fault::Adversary()->Reset();

            body();

            ::twist::rt::thr::fault::Adversary()->PrintReport();

            ::twist::rt::thr::log::FlushPendingLogMessages();
        }

    }  // namespace Stress
}  // namespace Testing::Twist

#else

// just threads

namespace Testing::Twist {
    namespace Stress {

        void Test(TestBody body, Params) {
            body();
        }

    }  // namespace Stress
}  // namespace Testing::Twist

#endif
