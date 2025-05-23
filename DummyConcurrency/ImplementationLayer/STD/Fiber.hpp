#pragma once

#include "DummyConcurrency/Context/StackView.hpp"

#include <span>

namespace NDummyConcurrency::NImplementationLayer::STD {

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

    inline void NewFiber(Fiber*, std::span<std::byte>) {}

    inline FiberHandle SwitchToFiber(FiberHandle handle) {
        return handle;
    }

}  // namespace NDummyConcurrency::NImplementationLayer::STD