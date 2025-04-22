#include <DummyConcurrency/DummyConcurrency.hpp>

#include <fmt/core.h>
#include <twist/ed/std/thread.hpp>
#include <twist/sim.hpp>
#include <twist/test/assert.hpp>
#include <wheels/test/framework.hpp>

static_assert(twist::build::IsolatedSim());

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(FiberSynchronizationMutex_Switch) {
    class SharedState {
    public:
        void Access() {
            ++access_count_;

            auto thread_id = twist::ed::std::this_thread::get_id();
            if (thread_id != last_accessor_id_) {
                ++switch_count_;
                last_accessor_id_ = thread_id;
            }
        }

        size_t AccessCount() const { return access_count_; }

        size_t SwitchCount() const { return switch_count_; }

    private:
        size_t                     access_count_ = 0;
        twist::ed::std::thread::id last_accessor_id_;
        size_t                     switch_count_ = 0;
    };

    void Test() {
        ThreadPool scheduler{4};
        scheduler.Start();

        SharedState state;

        TWaitGroup example;
        example.Add(1);

        const size_t kFibers = 512;
        const size_t kLocks  = 1024;

        Go(scheduler, [&] {
            FWaitGroup wg;

            FMutex mutex;

            for (size_t i = 0; i < kFibers; ++i) {
                wg.Add(1);

                Go([&] {
                    for (size_t j = 0; j < kLocks; ++j) {
                        std::lock_guard locker{mutex};

                        state.Access();

                        if (j % 3 == 0) {
                            twist::ed::std::this_thread::yield();
                        }
                    }

                    wg.Done();
                });
            }

            wg.Wait();

            example.Done();
        });

        example.Wait();

        scheduler.Stop();

        size_t access_count = state.AccessCount();
        size_t switch_count = state.SwitchCount();

        fmt::println("# critical sections = {}", access_count);
        fmt::println("# switches = {}", switch_count);

        TWIST_TEST_ASSERT(access_count == kFibers * kLocks, "Missing critical sections");

        const size_t kSwitchThreshold = 512;

        TWIST_TEST_ASSERT(switch_count < kSwitchThreshold, "Too many thread switches");
    }

    TEST(SwitchCount, wheels::test::TestOptions().TimeLimit(std::chrono::seconds(60))) {
        twist::sim::sched::RandomScheduler scheduler;
        twist::sim::Simulator              simulator{&scheduler};
        auto                               result = simulator.Run(Test);

        if (!result.Ok()) {
            fmt::println("Simulation status: {}", result.status);
            FAIL_TEST("Too many thread switches at shared state access");
        }
    }
}
