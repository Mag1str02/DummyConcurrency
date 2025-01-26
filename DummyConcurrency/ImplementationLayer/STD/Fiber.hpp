#pragma once

#include "DummyConcurrency/Context/StackView.hpp"

namespace DummyConcurrency::ImplementationLayer::STD {

    struct Fiber;
    struct FiberHandle;

    struct FiberHandle {
        FiberHandle() = default;
    };

    struct Fiber {
        Fiber() = default;

        Fiber(const Fiber&)            = delete;
        Fiber& operator=(const Fiber&) = delete;

        Fiber(Fiber&&)            = delete;
        Fiber& operator=(Fiber&&) = delete;

        FiberHandle Handle() { return FiberHandle(); }
    };

    inline void NewFiber(Fiber*, Context::StackView) {}

    inline FiberHandle SwitchToFiber(FiberHandle handle) {
        return handle;
    }

}  // namespace DummyConcurrency::ImplementationLayer::STD