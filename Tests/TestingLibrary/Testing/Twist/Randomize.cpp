#include "Randomize.hpp"

#include "Testing/TimeBudget.hpp"

#include <wheels/test/framework.hpp>

#include <fmt/core.h>

#if defined(__TWIST_SIM__)

// simulation

#include <twist/sim.hpp>

#if defined(__TWIST_SIM_ISOLATION__)

// matrix

#include "Testing/Twist/Simulation/Result.hpp"

namespace Testing::Twist {
    namespace Randomize {

        std::chrono::milliseconds TestTimeLimit(std::chrono::milliseconds budget) {
            // Reserve time for execution printing
            return budget + std::chrono::seconds(30);
        }

        using ::twist::sim::Simulator;
        using ::twist::sim::sched::PctScheduler;
        using ::twist::sim::sched::RandomScheduler;

        ::twist::sim::Result RunWithLimit(Simulator& simulator, TestBody body, size_t limit = 4096) {
            simulator.Start(body);
            size_t iters = simulator.RunFor(limit);

            if (iters >= limit) {
                simulator.Burn();

                // Print

                Simulator replay_simulator{simulator.Scheduler()};

                static const size_t kReplayIterLimit = 64;

                replay_simulator.Start(body);
                replay_simulator.Silent(true);
                ::twist::sim::StdoutPrinter printer;
                replay_simulator.SetLogger(&printer);
                replay_simulator.AllowSysLogging(true);
                replay_simulator.RunFor(kReplayIterLimit);

                fmt::println("... (output truncated after {} scheduling iterations)", kReplayIterLimit);

                replay_simulator.Burn();

                std::cout.flush();

                FAIL_TEST(fmt::format("Execution took too long (> {} scheduling iterations) and was interrupted", limit));

                std::abort();

            } else {
                return simulator.Drain();  // ~ Stop runloop
            }
        }

        size_t EstimateSize(TestBody body) {
            ::twist::sim::sched::RandomScheduler scheduler;
            ::twist::sim::Simulator              simulator{&scheduler};
            auto                                 result = RunWithLimit(simulator, body);
            return result.iters;
        };

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

        void Check(TestBody body, Params params) {
            if (!::twist::sim::DetCheck(body)) {
                FAIL_TEST("Test routine is not deterministic");
            }

            {
                static const size_t kTestSizeLimit = 10'000;

                if (EstimateSize(body) > kTestSizeLimit) {
                    FAIL_TEST("Test is too large for randomized testing: >" << kTestSizeLimit << " steps");
                }
            }

            Testing::TimeBudget time_budget{params.TimeBudget};

            auto sched_params       = RandomScheduler::Params{};
            sched_params.time_slice = 13;
            RandomScheduler scheduler{sched_params};

            size_t sim_count = 0;

            do {
                Simulator simulator{&scheduler};

                auto result = RunWithLimit(simulator, body);

                ++sim_count;

                if (result.Failure()) {
                    static const size_t kNotTooLarge = 256;

                    if (result.iters < kNotTooLarge) {
                        auto replay = TryRecord(body, scheduler);
                        if (replay.Digest != result.digest) {
                            WHEELS_PANIC("Twist error: failed to record simulation replay");
                        }

                        ::twist::sim::Print(body, replay.Schedule);
                        std::cout.flush();
                    }

                    FAIL_TEST(Simulation::FormatFailure(result));
                }
            } while (scheduler.NextSchedule() && time_budget);

            fmt::println("Executions checked: {}", sim_count);
        }

    }  // namespace Randomize
}  // namespace Testing::Twist

#else

// Simulation, maybe with sanitizers

namespace Testing::Twist {
    namespace Randomize {

        std::chrono::milliseconds TestTimeLimit(std::chrono::milliseconds budget) {
            return budget + std::chrono::seconds(1);
        }

        using ::twist::sim::sched::PctScheduler;
        using ::twist::sim::sched::RandomScheduler;

        size_t EstimateSize(TestBody body) {
            ::twist::sim::sched::RandomScheduler scheduler;
            ::twist::sim::Simulator              simulator{&scheduler};
            auto                                 result = simulator.Run(body);
            return result.iters;
        };

        void Check(TestBody body, Params params) {
            // TODO: PctScheduler

            if (EstimateSize(body) > 1024) {
                FAIL_TEST("Test is too large for random checking");
            }

            auto sched_params       = RandomScheduler::Params{};
            sched_params.time_slice = 13;
            RandomScheduler scheduler{sched_params};

            size_t sim_count = 0;

            Testing::TimeBudget time_budget{params.TimeBudget};

            do {
                ::twist::sim::Simulator simulator{&scheduler};
                auto                    result = simulator.Run(body);

                ++sim_count;

                if (result.Failure()) {
                    // Impossible?
                }

            } while (scheduler.NextSchedule() && time_budget);

            fmt::println("Executions checked: {}", sim_count);
        }

    }  // namespace Randomize
}  // namespace Testing::Twist

#endif

#elif defined(__TWIST_FAULTY__)

// threads + fault injection

#include <twist/rt/thr/fault/adversary/adversary.hpp>

#include <twist/rt/thr/logging/logging.hpp>

#include <twist/test/new_iter.hpp>

namespace Testing::Twist {
    namespace Randomize {

        std::chrono::milliseconds TestTimeLimit(std::chrono::milliseconds budget) {
            return budget + std::chrono::seconds(1);
        }

        void TestIter(size_t iter, TestBody body, Params) {
            body();

            ::twist::rt::thr::log::FlushPendingLogMessages();

            ::twist::rt::thr::fault::Adversary()->Iter(iter);
        }

        void Check(TestBody body, Params params) {
            ::twist::rt::thr::fault::Adversary()->Reset();

            Testing::TimeBudget time_budget{params.TimeBudget};

            size_t count = 0;

            while (time_budget) {
                TestIter(count, body, params);
                ++count;
            }

            fmt::println("Executions checked: {}", count);
            ::twist::rt::thr::fault::Adversary()->PrintReport();
            std::cout.flush();
        }

    }  // namespace Randomize
}  // namespace Testing::Twist

#else

// just threads

namespace Testing::Twist {
    namespace Randomize {

        void Check(TestBody, Params) {
            WHEELS_PANIC("Random checking is not supported for this build");
        }

    }  // namespace Randomize
}  // namespace Testing::Twist

#endif
