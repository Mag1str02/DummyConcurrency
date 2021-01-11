#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(FiberScheduling_UnitRL) {
    SIMPLE_TEST(Go) {
        RunLoop loop;

        Go(loop, [] {});

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, 1);
    }

    SIMPLE_TEST(GoGroup) {
        RunLoop loop;

        const size_t kFibers = 7;

        for (size_t i = 0; i < kFibers; ++i) {
            Go(loop, [] {});
        }

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, kFibers);
    }

    SIMPLE_TEST(GoChild) {
        RunLoop loop;

        bool flag = false;

        Go(loop, [&] { Go([&] { flag = true; }); });

        ASSERT_TRUE(loop.RunNext());
        ASSERT_FALSE(flag);
        ASSERT_EQ(loop.Run(), 1);
        ASSERT_TRUE(flag);
    }

    SIMPLE_TEST(Yield) {
        RunLoop loop;

        Go(loop, [] { Yield(); });

        ASSERT_EQ(loop.Run(), 2);
    }

    SIMPLE_TEST(PingPong) {
        RunLoop loop;

        int turn = 0;

        Go(loop, [&] {
            for (size_t i = 0; i < 3; ++i) {
                ASSERT_EQ(turn, 0);
                turn ^= 1;
                Yield();
            }
        });

        Go(loop, [&] {
            for (size_t j = 0; j < 3; ++j) {
                ASSERT_EQ(turn, 1);
                turn ^= 1;
                Yield();
            }
        });

        loop.Run();
    }

    SIMPLE_TEST(YieldGroup) {
        RunLoop loop;

        const size_t kFibers = 3;
        const size_t kYields = 4;

        for (size_t i = 0; i < kFibers; ++i) {
            Go(loop, [] {
                for (size_t k = 0; k < kYields; ++k) {
                    Yield();
                }
            });
        }

        size_t tasks = loop.Run();
        ASSERT_EQ(tasks, kFibers * (kYields + 1));
    }
}
