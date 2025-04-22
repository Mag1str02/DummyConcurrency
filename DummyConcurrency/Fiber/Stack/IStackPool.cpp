#include "IStackPool.hpp"

namespace DummyConcurrency::Fiber {

    NewStack* LeasedStack::operator->() {
        return &stack_;
    }
    LeasedStack::LeasedStack(LeasedStack&& other) : stack_(std::move(other.stack_)), pool_(other.pool_) {
        other.pool_ = nullptr;
    }
    LeasedStack::~LeasedStack() {
        if (pool_ != nullptr) {
            pool_->FreeStack(std::move(stack_));
        }
    }

    LeasedStack::LeasedStack(NewStack&& stack, IStackPool* pool) : stack_(std::move(stack)), pool_(pool) {}

    LeasedStack IStackPool::GetStack() {
        return LeasedStack(AllocateStack(), this);
    };

}  // namespace DummyConcurrency::Fiber
