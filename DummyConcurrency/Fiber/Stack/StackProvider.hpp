#pragma once

#include "New.hpp"

#include <utility>

namespace NDummyConcurrency::NFiber {
    class LeasedStack;

    class IStackProvider {
    public:
        LeasedStack GetStack();

    protected:
        virtual NewStack AllocateStack()             = 0;
        virtual void     FreeStack(NewStack&& stack) = 0;

    private:
        friend class LeasedStack;
    };

    class LeasedStack : NonCopyable {
    public:
        NewStack* operator->() { return &stack_; }
        LeasedStack(LeasedStack&& other) : stack_(std::move(other.stack_)), pool_(other.pool_) { other.pool_ = nullptr; }
        LeasedStack& operator=(LeasedStack&& other) = delete;
        ~LeasedStack() {
            if (pool_ != nullptr) {
                pool_->FreeStack(std::move(stack_));
            }
        }

    private:
        friend class IStackProvider;
        explicit LeasedStack(NewStack&& stack, IStackProvider* pool) : stack_(std::move(stack)), pool_(pool) {}

    private:
        NewStack        stack_;
        IStackProvider* pool_;
    };

}  // namespace NDummyConcurrency::NFiber
