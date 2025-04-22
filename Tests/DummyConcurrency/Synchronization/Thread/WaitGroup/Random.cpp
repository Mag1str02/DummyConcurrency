#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

#include <twist/assist/random.hpp>
#include <twist/assist/shared.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/test/assert.hpp>

#include <array>
#include <vector>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(ThreadSynchronizationWaitGroup_Random) {
    TWIST_RANDOMIZE(Jobs, 5s) {
        twist::ed::std::random_device rd{};
        twist::assist::Choice         choice{rd};

        constexpr size_t kMaxJobs    = 5;
        constexpr size_t kMaxWaiters = 5;

        size_t workers = choice(1, kMaxJobs);
        size_t waiters = choice(1, kMaxWaiters);

        std::array<twist::assist::Shared<bool>, kMaxJobs> jobs;

        for (size_t i = 0; i < kMaxJobs; ++i) {
            jobs[i].Write(false);
        }

        std::vector<twist::ed::std::thread> threads;

        TWaitGroup wg;

        for (size_t i = 0; i < workers; ++i) {
            wg.Add(1);

            threads.emplace_back([&, i] {
                jobs[i].Write(true);
                wg.Done();
            });
        }

        for (size_t j = 0; j < waiters; ++j) {
            threads.emplace_back([&] {
                wg.Wait();

                for (size_t i = 0; i < workers; ++i) {
                    TWIST_TEST_ASSERT(jobs[i], "Unfinished work");
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }
    }

    TWIST_RANDOMIZE(ConcurrentAddWait, 5s) {
        std::vector<twist::ed::std::thread> threads;

        TWaitGroup wg;

        twist::assist::Shared<bool> flag{false};

        twist::ed::std::thread t2;

        wg.Add(1);
        twist::ed::std::thread t1([&] {
            wg.Add(1);
            t2 = twist::ed::std::thread([&] {
                flag.Write(true);
                wg.Done();
            });
            wg.Done();
        });

        wg.Wait();

        auto f = flag.Read();
        TWIST_TEST_ASSERT(f, "Missing Add");

        t1.join();
        t2.join();
    }
}
