#include <DummyConcurrency/DummyConcurrency.hpp>

#include <fmt/core.h>
#include <wheels/test/framework.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(SchedulingRunLoop_Unit) {
    SIMPLE_TEST(JustWorks) {
        RunLoop loop;

        size_t step = 0;

        ASSERT_FALSE(loop.NonEmpty());

        ASSERT_FALSE(loop.RunNext());
        ASSERT_EQ(loop.RunAtMost(99), 0);

        Submit(loop, [&] { step = 1; });

        ASSERT_TRUE(loop.NonEmpty());

        ASSERT_EQ(step, 0);

        Submit(loop, [&] { step = 2; });

        ASSERT_EQ(step, 0);

        ASSERT_TRUE(loop.RunNext());

        ASSERT_EQ(step, 1);

        ASSERT_TRUE(loop.NonEmpty());

        Submit(loop, [&] { step = 3; });

        ASSERT_EQ(loop.RunAtMost(99), 2);
        ASSERT_EQ(step, 3);

        ASSERT_FALSE(loop.NonEmpty());
        ASSERT_FALSE(loop.RunNext());
    }

    SIMPLE_TEST(Empty) {
        RunLoop loop;

        ASSERT_FALSE(loop.RunNext());
        ASSERT_EQ(loop.RunAtMost(7), 0);
        ASSERT_EQ(loop.Run(), 0);
    }

    void Countdown(RunLoop & loop, size_t k) {
        if (k > 0) {
            Submit(loop, [&loop, k] { Countdown(loop, k - 1); });
        }
    }

    SIMPLE_TEST(RunAtMost) {
        RunLoop loop;

        Countdown(loop, 256);

        size_t tasks = 0;
        do {
            tasks += loop.RunAtMost(7);
        } while (loop.NonEmpty());

        fmt::println("{}", tasks);

        ASSERT_EQ(tasks, 256);
    }

    SIMPLE_TEST(RunAtMostNewTasks) {
        RunLoop loop;

        Submit(loop, [&]() { Submit(loop, []() {}); });

        ASSERT_EQ(loop.RunAtMost(2), 2);
    }

    SIMPLE_TEST(Run) {
        RunLoop loop;

        Countdown(loop, 117);

        ASSERT_EQ(loop.Run(), 117);
    }

    SIMPLE_TEST(RunTwice) {
        RunLoop loop;

        Countdown(loop, 11);

        ASSERT_EQ(loop.Run(), 11);

        Countdown(loop, 7);

        ASSERT_EQ(loop.Run(), 7);
    }
}
