#include "Coroutine.hpp"

namespace DummyConcurrency::Fiber {

    ICoroutine::ICoroutine(size_t stack_size) : stack_(Stack::AllocateBytes(stack_size)) {
        coro_context_.Setup(stack_.MutView(), this);
        caller_context_ = nullptr;
        ImplementationLayer::NewFiber(&impl_fiber_, stack_.MutView());
    }

    void ICoroutine::SwitchTo(ICoroutine& other) {
        if (this == &other) {
            return;
        }

        std::swap(impl_caller_fiber_, other.impl_caller_fiber_);
        std::swap(caller_context_, other.caller_context_);

        ImplementationLayer::SwitchToFiber(other.impl_fiber_.Handle());
        coro_context_.SwitchTo(other.coro_context_);
    }

    void ICoroutine::Resume() {
        Context::ExecutionContext        caller_context;
        ImplementationLayer::FiberHandle impl_caller_fiber;

        caller_context_    = &caller_context;
        impl_caller_fiber_ = &impl_caller_fiber;

        impl_caller_fiber = ImplementationLayer::SwitchToFiber(impl_fiber_.Handle());
        caller_context_->SwitchTo(coro_context_);
    }

    void ICoroutine::Suspend() {
        ImplementationLayer::SwitchToFiber(*impl_caller_fiber_);
        coro_context_.SwitchTo(*caller_context_);
    }

    bool ICoroutine::IsCompleted() const {
        return completed_;
    }

}  // namespace DummyConcurrency::Fiber
