#include "Coroutine.hpp"

namespace DummyConcurrency::Fiber {

    ICoroutine::ICoroutine(size_t stack_size) : stack_(Stack::AllocateBytes(stack_size)) {
        coro_context_.Setup(stack_.MutView(), this);
        caller_context_ = nullptr;
        twist::assist::NewFiber(&twist_fiber_, stack_.MutView());
    }

    void ICoroutine::SwitchTo(ICoroutine& other) {
        if (this == &other) {
            return;
        }

        std::swap(twist_caller_fiber_, other.twist_caller_fiber_);
        std::swap(caller_context_, other.caller_context_);

        twist::assist::SwitchToFiber(other.twist_fiber_.Handle());
        coro_context_.SwitchTo(other.coro_context_);
    }

    void ICoroutine::Resume() {
        Context::ExecutionContext     caller_context;
        twist::assist::FiberHandle twist_caller_fiber;

        caller_context_     = &caller_context;
        twist_caller_fiber_ = &twist_caller_fiber;

        twist_caller_fiber = twist::assist::SwitchToFiber(twist_fiber_.Handle());
        caller_context_->SwitchTo(coro_context_);
    }

    void ICoroutine::Suspend() {
        twist::assist::SwitchToFiber(*twist_caller_fiber_);
        coro_context_.SwitchTo(*caller_context_);
    }

    bool ICoroutine::IsCompleted() const {
        return completed_;
    }

}  // namespace DummyConcurrency::Fiber
