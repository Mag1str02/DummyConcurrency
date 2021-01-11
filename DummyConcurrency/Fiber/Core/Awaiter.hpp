#pragma once

#include "Handle.hpp"

#include <DummyConcurrency/Common.hpp>

namespace DummyConcurrency::Fiber {

    class IAwaiter {
    public:
        virtual ~IAwaiter() = default;

        void Schedule() {
            DC_ASSERT(handle_.IsValid(), "Invalid fiber handle");
            handle_.Schedule();
        }
        void Switch(FiberHandle& current_fiber) {
            DC_ASSERT(handle_.IsValid(), "Invalid fiber handle");
            handle_.Switch(current_fiber);
        }

    protected:
        virtual void OnSuspend() noexcept {};

    private:
        void Suspend(FiberHandle handle) noexcept {
            handle_ = std::move(handle);
            OnSuspend();
        }

        friend class Fiber;

        FiberHandle handle_;
    };

}  // namespace DummyConcurrency::Fiber
