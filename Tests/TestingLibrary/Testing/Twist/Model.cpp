#include "Model.hpp"

#include "Testing/TimeBudget.hpp"
#include "Testing/Twist/Simulation/Result.hpp"

#include <twist/sim.hpp>
#include <wheels/core/panic.hpp>
#include <wheels/test/framework.hpp>

#include <fmt/core.h>

namespace Testing::Twist {

    namespace Model {

        struct ExploreResult {
            std::optional<::twist::sim::Result> Failure;
            size_t                              SimCount;
        };

        using ModelChecker = ::twist::sim::sched::DfsScheduler;
        using Simulator    = ::twist::sim::Simulator;

        static ModelChecker::Params ToModelCheckerParams(Model::Params test_params) {
            ModelChecker::Params params;

            params.max_preemptions   = test_params.MaxPreemptions;
            params.max_steps         = test_params.MaxSteps;
            params.spurious_failures = test_params.SpuriousFailures;
            params.spurious_wakeups  = test_params.SpuriousWakeups;

            return params;
        }

        ::twist::sim::Result RunWithLimit(Simulator& simulator, TestBody body, size_t limit = 4096) {
            simulator.Start(body);
            size_t iters = simulator.RunFor(limit);

            if (iters >= limit) {
                simulator.Burn();

                // Print

                static const size_t kReplayIterLimit = 64;

                Simulator replay_simulator{simulator.Scheduler()};

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

        ExploreResult Explore(TestBody body, ModelChecker& checker) {
            size_t sim_count = 0;

            Testing::TimeBudget time_budget;

            do {
                ++sim_count;

                ::twist::sim::Simulator simulator{&checker};

                auto result = RunWithLimit(simulator, body);

                // fmt::println("Steps: {}", result.iters);

                if (result.Failure()) {
                    return {result, sim_count};
                }

                if (!time_budget) {
                    FAIL_TEST(
                        fmt::format("Time budget for model checking is over: too many executions (> {}). Try to optimize your solution", sim_count));
                }

            } while (checker.NextSchedule());

            return {{}, sim_count};  // Ok
        }

        struct Replay {
            ::twist::sim::sched::Schedule Schedule;
            ::twist::sim::Digest          Digest;
        };

        Replay TryRecord(TestBody body, ModelChecker& checker) {
            ::twist::sim::sched::Recorder recorder{&checker};
            ::twist::sim::Simulator       simulator{&recorder};
            auto                          result = simulator.Run(body);
            return {recorder.GetSchedule(), result.digest};
        }

        size_t EstimateSize(TestBody body) {
            ::twist::sim::sched::RandomScheduler scheduler;
            ::twist::sim::Simulator              simulator{&scheduler};
            auto                                 result = RunWithLimit(simulator, body);
            return result.iters;
        };

        void Check(TestBody body, Params test_params) {
            if (!::twist::sim::DetCheck(body)) {
                FAIL_TEST("Test routine is not deterministic");
            }

            {
                static const size_t kTestSizeLimit = 512;
                size_t              size           = EstimateSize(body);
                if (size > kTestSizeLimit) {
                    FAIL_TEST("Test is too large for model checking");
                }
            }

            ModelChecker checker{ToModelCheckerParams(test_params)};

            // Explore test
            auto exp = Explore(body, checker);

            if (exp.Failure) {
                ::twist::sim::Result failure = *exp.Failure;

                auto replay = TryRecord(body, checker);

                if (replay.Digest != failure.digest) {
                    WHEELS_PANIC("Twist error: failed to record simulation replay");
                }

                ::twist::sim::Print(body, replay.Schedule);

                FAIL_TEST(Twist::Simulation::FormatFailure(failure));

            } else {
                // TODO: print report
                fmt::println("Model checking completed: {} executions", exp.SimCount);

                return;  // Passed
            }
        }

    }  // namespace Model

}  // namespace Testing::Twist
