#include <DummyConcurrency/DummyConcurrency.hpp>

#include <wheels/test/framework.hpp>

#include <string>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(LockFreeStack) {
    SIMPLE_TEST(JustWorks) {
        LockFreeStack<std::string> stack;

        stack.Push("Data");
        auto item = stack.TryPop();
        ASSERT_TRUE(item);
        ASSERT_EQ(*item, "Data");

        auto empty = stack.TryPop();
        ASSERT_FALSE(empty);
    }

    SIMPLE_TEST(LIFO) {
        LockFreeStack<int> stack;

        stack.Push(1);
        stack.Push(2);
        stack.Push(3);

        ASSERT_EQ(*stack.TryPop(), 3);
        ASSERT_EQ(*stack.TryPop(), 2);
        ASSERT_EQ(*stack.TryPop(), 1);

        ASSERT_FALSE(stack.TryPop());
    }

    SIMPLE_TEST(Dtor) {
        LockFreeStack<std::string> stack;
        stack.Push("Hello");
        stack.Push("World");
    }

    struct MoveOnly {
        MoveOnly() = default;

        // Movable
        MoveOnly(MoveOnly&&)            = default;
        MoveOnly& operator=(MoveOnly&&) = default;

        // Non-copyable
        MoveOnly(const MoveOnly&)            = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
    };

    SIMPLE_TEST(MoveOnlyValue) {
        LockFreeStack<MoveOnly> stack;
        stack.Push({});
        ASSERT_TRUE(stack.TryPop());
    }

    struct NonDefaultConstructible {
        NonDefaultConstructible(int) {}
    };

    SIMPLE_TEST(NonDefaultConstructibleValue) {
        LockFreeStack<NonDefaultConstructible> stack;
        stack.Push(NonDefaultConstructible{7});
        ASSERT_TRUE(stack.TryPop());
    }

    SIMPLE_TEST(TwoStacks) {
        LockFreeStack<int> stack_1;
        LockFreeStack<int> stack_2;

        stack_1.Push(3);
        stack_2.Push(11);
        ASSERT_EQ(*stack_1.TryPop(), 3);
        ASSERT_EQ(*stack_2.TryPop(), 11);
    }
}
