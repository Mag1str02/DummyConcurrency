#include "Coroutine.hpp"

#include <DummyConcurrency/Utils/Assert.hpp>

namespace NDummyConcurrency::NFiber {

    ICoroutine::ICoroutine(NContext::StackView view) {
        DC_PROFILE_SCOPE();
        coro_context_.Setup(view, this);
        caller_context_ = nullptr;
        NImplementationLayer::NewFiber(&impl_fiber_, {view.Bottom, size_t(view.Top - view.Bottom)});
    }

    void ICoroutine::SwitchTo(ICoroutine& other) {
        DC_PROFILE_SCOPE();
        if (this == &other) {
            return;
        }

        std::swap(impl_caller_fiber_, other.impl_caller_fiber_);
        std::swap(caller_context_, other.caller_context_);

        NImplementationLayer::SwitchToFiber(other.impl_fiber_.Handle());
        coro_context_.SwitchTo(other.coro_context_);
    }

    void ICoroutine::Resume() {
        DC_PROFILE_SCOPE();
        NContext::ExecutionContext        caller_context;
        NImplementationLayer::FiberHandle impl_caller_fiber;

        caller_context_    = &caller_context;
        impl_caller_fiber_ = &impl_caller_fiber;

        impl_caller_fiber = NImplementationLayer::SwitchToFiber(impl_fiber_.Handle());
        caller_context_->SwitchTo(coro_context_);
    }

    void ICoroutine::Suspend() {
        DC_PROFILE_SCOPE();
        NImplementationLayer::SwitchToFiber(*impl_caller_fiber_);
        coro_context_.SwitchTo(*caller_context_);
    }

    bool ICoroutine::IsCompleted() const {
        return completed_;
    }

    NImplementationLayer::Fiber* ICoroutine::GetFiber() {
        return &impl_fiber_;
    }

    void ICoroutine::Run() noexcept {
        try {
            DC_PROFILE_SCOPE();
            Body();
        } catch (const std::exception& exception) {
            auto msg = std::format("Exceptions are forbidden in coroutins: '{}'", exception.what());
            DC_PANIC(msg);
        } catch (...) {
            DC_PANIC("Exceptions are forbidden in coroutins");
        }
        NImplementationLayer::SwitchToFiber(*impl_caller_fiber_);

        completed_ = true;
        coro_context_.ExitTo(*caller_context_);
        // Unreachable
    }

}  // namespace NDummyConcurrency::NFiber
