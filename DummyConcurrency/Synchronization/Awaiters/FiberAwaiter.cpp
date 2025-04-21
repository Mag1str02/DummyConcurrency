#include "FiberAwaiter.hpp"

#include <DummyConcurrency/Fiber/Core/Fiber.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Synchronization {

    static thread_local Fiber::Handle gSwitchFiberBuffer;

    void IFiberAwaiter::Wake() noexcept {
        if (Fiber::Fiber::Self() != nullptr) {
            Switch(gSwitchFiberBuffer);
        } else {
            Schedule();
        }
    }
    void IFiberAwaiter::AfterSuspend() {
        if (gSwitchFiberBuffer.IsValid()) {
            gSwitchFiberBuffer.Schedule();
        }
    }

}  // namespace DummyConcurrency::Synchronization
