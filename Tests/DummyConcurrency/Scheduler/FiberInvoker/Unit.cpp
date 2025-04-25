#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/core/stop_watch.hpp>
#include <wheels/test/framework.hpp>
#include <wheels/test/util/cpu_timer.hpp>

#include <chrono>  // IWYU pragma: keep

using namespace std::chrono_literals;  // NOLINT
using namespace NDummyConcurrency;     // NOLINT

TEST_SUITE(SchedulingFiberInvoker_Unit) {
    SIMPLE_TEST(RunLoop) {
        StackPool    pool;
        RunLoop      loop;
        FiberInvoker invoker(&pool, &loop);
        loop.SetInvoker(&invoker);

        constexpr uint64_t kUserAmount = 5;

        for (uint64_t i = 0; i < kUserAmount; ++i) {
            NRuntime::Submit(loop, []() {
                for (size_t i = 0; i < 100; ++i) {
                    Yield();
                }
            });
        }
        loop.Run();

        std::println("Pool size: {}", pool.Size());
        std::println("Pool leased amount: {}", pool.LeasedAmount());

        ASSERT_EQ(kUserAmount + 1, pool.Size());
        ASSERT_EQ(0, pool.LeasedAmount());
    }
    SIMPLE_TEST(ThreadPool) {
        constexpr uint64_t kUserAmount = 5;
        StackPool          pool;
        ThreadPool         thread_pool(kUserAmount);
        FiberInvoker       invoker(&pool, &thread_pool);
        thread_pool.SetInvoker(&invoker);

        TWaitGroup wg;

        wg.Add(kUserAmount);
        for (uint64_t i = 0; i < kUserAmount; ++i) {
            NRuntime::Submit(thread_pool, [&]() {
                for (size_t i = 0; i < 10000; ++i) {
                    Yield();
                }
                wg.Done();
            });
        }
        thread_pool.Start();
        wg.Wait();
        thread_pool.Stop();

        std::println("Pool size: {}", pool.Size());
        std::println("Pool leased amount: {}", pool.LeasedAmount());

        ASSERT_EQ(kUserAmount * 2, pool.Size());
        ASSERT_EQ(0, pool.LeasedAmount());
    }
}
