#pragma once

#include "Forward.hpp"

namespace NDummyConcurrency::NFiber {

    // Opaque non-owning handle to the _suspended_ fiber

    class Handle {
        friend class Fiber;

    public:
        Handle()                         = default;
        Handle(const Handle&)            = delete;
        Handle& operator=(const Handle&) = delete;
        Handle(Handle&& other) : fiber_(other.fiber_) { other.fiber_ = nullptr; };
        Handle& operator=(Handle&& other) {
            if (this == &other) {
                return *this;
            }
            fiber_       = other.fiber_;
            other.fiber_ = nullptr;
            return *this;
        };

        static Handle Invalid() { return Handle(nullptr); }

        bool IsValid() const { return fiber_ != nullptr; }

        // Schedule fiber to the associated scheduler
        void Schedule();

        // Switch to this fiber immediately
        // For symmetric transfer
        void Switch(Handle& current_fiber);

        Fiber* Release();

    private:
        explicit Handle(Fiber* fiber) : fiber_(fiber) {}

    private:
        Fiber* fiber_ = nullptr;
    };

}  // namespace NDummyConcurrency::NFiber
