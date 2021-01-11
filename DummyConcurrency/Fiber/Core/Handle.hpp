#pragma once

#include "Forward.hpp"

namespace DummyConcurrency::Fiber {

    // Opaque non-owning handle to the _suspended_ fiber

    class FiberHandle {
        friend class Fiber;

    public:
        FiberHandle()                              = default;
        FiberHandle(const FiberHandle&)            = delete;
        FiberHandle& operator=(const FiberHandle&) = delete;
        FiberHandle(FiberHandle&& other) : fiber_(other.fiber_) { other.fiber_ = nullptr; };
        FiberHandle& operator=(FiberHandle&& other) {
            if (this == &other) {
                return *this;
            }
            fiber_       = other.fiber_;
            other.fiber_ = nullptr;
            return *this;
        };

        static FiberHandle Invalid() { return FiberHandle(nullptr); }

        bool IsValid() const { return fiber_ != nullptr; }

        // Schedule fiber to the associated scheduler
        void Schedule();

        // Switch to this fiber immediately
        // For symmetric transfer
        void Switch(FiberHandle& current_fiber);

        Fiber* Release();

    private:
        explicit FiberHandle(Fiber* fiber) : fiber_(fiber) {}

    private:
        Fiber* fiber_ = nullptr;
    };

}  // namespace DummyConcurrency::Fiber
