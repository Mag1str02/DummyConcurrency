#pragma once

#include "Handle.hpp"

#include <DummyConcurrency/Utils/Assert.hpp>

namespace NDummyConcurrency::NFiber {

    class IAwaiter {
    public:
        virtual ~IAwaiter() = default;

        void Schedule() {
            DC_ASSERT(handle_.IsValid(), "Invalid fiber handle");
            handle_.Schedule();
        }
        void Switch(Handle& current_fiber) {
            DC_ASSERT(handle_.IsValid(), "Invalid fiber handle");
            handle_.Switch(current_fiber);
        }

    protected:
        virtual void OnSuspend() noexcept {};

    private:
        void Suspend(Handle handle) noexcept {
            handle_ = std::move(handle);
            OnSuspend();
        }

        friend class Fiber;

        Handle handle_;
    };

}  // namespace NDummyConcurrency::NFiber
