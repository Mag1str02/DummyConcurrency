#include "StackPool.hpp"

namespace DummyConcurrency::Fiber {

    StackPool::StackPool(StackSize size) : stack_size_(SizeInBytes(size)) {}

    NewStack StackPool::AllocateStack() {
        std::unique_lock guard(lock_);
        if (stack_of_stack_ != nullptr) {
            guard.unlock();
            return NewStack::Allocate(stack_size_);
        }
        NewStack stack  = std::move(stack_of_stack_->Stack);
        stack_of_stack_ = stack_of_stack_->Next;
        size_.fetch_add(1);
        return stack;
    }
    void StackPool::FreeStack(NewStack&& stack) {
        stack.Reset();
        NewNode* node = stack.Allocate<NewNode>(std::move(stack));  // This is very risky but should work
        size_.fetch_add(1);

        std::unique_lock guard(lock_);
        node->Next      = stack_of_stack_;
        stack_of_stack_ = node;
    }
    void StackPool::Clear() {
        std::unique_lock guard(lock_);
        while (stack_of_stack_ != nullptr) {
            NewNode* next = stack_of_stack_->Next;
            stack_of_stack_->Stack.~NewStack();
            stack_of_stack_ = next;
        }
    }
    uint64_t StackPool::Size() const {
        return size_.load();
    }

    StackPool::NewNode::NewNode(NewStack&& stack) : Stack(std::move(stack)) {}
}  // namespace DummyConcurrency::Fiber
