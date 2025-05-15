#pragma once

#include <DummyConcurrency/Utils/Traits.hpp>

#include <cstdint>
#include <span>

namespace NDummyConcurrency::NImplementationLayer::NDummyEngine {

    class Fiber;
    class FiberHandle;

    class FiberHandle {
    public:
        FiberHandle() = default;
        explicit FiberHandle(Fiber* fiber) : fiber_(fiber) {}

    private:
        friend FiberHandle SwitchToFiber(FiberHandle handle);

        Fiber* fiber_ = nullptr;
    };

    class Fiber : public NonCopyable, public NonMovable {
    public:
        Fiber() = default;

        FiberHandle Handle() { return FiberHandle(this); }
        void        SetName(const char* name, int32_t group = 0) {
            name_  = name;
            group_ = group;
        }

    private:
        friend class FiberHandle;
        friend FiberHandle SwitchToFiber(FiberHandle handle);

        const char* name_  = nullptr;
        int32_t     group_ = 0;
    };

    inline void NewFiber(Fiber*, std::span<std::byte>) {}

    FiberHandle SwitchToFiber(FiberHandle handle);

}  // namespace NDummyConcurrency::NImplementationLayer::NDummyEngine