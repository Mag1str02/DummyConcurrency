#include "StackPool.hpp"

#include <vector>

namespace NDummyConcurrency::NFiber {

    StackPool::StackPool(StackSize size, uint64_t preallocate_count) : stack_size_(SizeInBytes(size)) {
        if (preallocate_count == 0) {
            return;
        }
        std::vector<NewStack> stacks;
        stacks.reserve(preallocate_count);
        for (uint64_t i = 0; i < preallocate_count; ++i) {
            stacks.emplace_back(AllocateStack());
        }
        for (auto& stack : stacks) {
            FreeStack(std::move(stack));
        }
    }
    StackPool::~StackPool() {
        Clear();
    }

    NewStack StackPool::AllocateStack() {
        lock_.Lock();
        if (stack_of_stack_ == nullptr) {
            lock_.Unlock();
            size_.fetch_add(1);
            return NewStack::Allocate(stack_size_);
        }
        NewStack stack  = std::move(stack_of_stack_->Stack);
        stack_of_stack_ = stack_of_stack_->Next;
        lock_.Unlock();
        return stack;
    }
    void StackPool::FreeStack(NewStack&& stack) {
        stack.Reset();
        void*    node_addr = stack.PreAllocate<NewNode>();
        NewNode* node      = new (node_addr) NewNode(std::move(stack));

        lock_.Lock();
        node->Next      = stack_of_stack_;
        stack_of_stack_ = node;
        lock_.Unlock();
    }
    void StackPool::Clear() {
        lock_.Lock();
        size_.store(0);
        while (stack_of_stack_ != nullptr) {
            NewNode* next = stack_of_stack_->Next;
            stack_of_stack_->Stack.~NewStack();
            stack_of_stack_ = next;
        }
        lock_.Unlock();
    }
    uint64_t StackPool::Size() const {
        return size_.load();
    }

    StackPool::NewNode::NewNode(NewStack&& stack) : Stack(std::move(stack)) {}
}  // namespace NDummyConcurrency::NFiber
