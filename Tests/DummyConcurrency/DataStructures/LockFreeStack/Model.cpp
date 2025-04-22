#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Checksum.hpp>
#include <Testing/Twist.hpp>

#include <twist/test/assert.hpp>
#include <twist/test/message.hpp>
#include <twist/test/wg.hpp>

static_assert(twist::build::IsolatedSim());

using TestMessage = twist::test::Message<int>;
using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(ModelLockFreeStack) {
    // 3 threads, Push + TryPop, <= 2 preemptions

    constexpr auto kPushPop1Params = Testing::Twist::Model::Params{.MaxPreemptions = 2};

    TWIST_MODEL(PushPop1, kPushPop1Params) {
        LockFreeStack<TestMessage> stack;

        Testing::CheckSum<int> s;

        twist::test::WaitGroup wg;

        wg.Add(3, [&](size_t index) {
            {
                int d = (int)index;

                stack.Push(TestMessage::New(d));
                // Checksum
                s.Produce(d);
            }

            {
                auto m = stack.TryPop();
                TWIST_TEST_ASSERT(m, "Expected message");
                int d = m->Read();
                s.Consume(d);
            }
        });

        wg.Join();

        TWIST_TEST_ASSERT(s.Validate(), "Checksum mismatch");
    }

    // 2 threads, Push + TryPop, <= 4 preemptions

    constexpr auto kPushPop2Params = Testing::Twist::Model::Params{.MaxPreemptions = 4};

    TWIST_MODEL(PushPop2, kPushPop2Params) {
        LockFreeStack<TestMessage> stack;

        Testing::CheckSum<int> s;

        twist::test::WaitGroup wg;

        wg.Add(2, [&](size_t index) {
            {
                int d = (int)index;

                stack.Push(TestMessage::New(d));
                // Checksum
                s.Produce(d);
            }

            {
                auto m = stack.TryPop();
                TWIST_TEST_ASSERT(m, "Expected message");
                int d = m->Read();
                s.Consume(d);
            }
        });

        wg.Join();

        TWIST_TEST_ASSERT(s.Validate(), "Checksum mismatch");
    }
}
