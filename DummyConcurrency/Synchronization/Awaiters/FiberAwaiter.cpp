#include "FiberAwaiter.hpp"

#include <DummyConcurrency/Fiber/Core/Fiber.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace NDummyConcurrency::NSynchronization {

    static thread_local NFiber::Handle gSwitchFiberBuffer;

    void IFiberAwaiter::Wake() noexcept {
        if (symmetric_switch_ && NFiber::Fiber::Self() != nullptr) {
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
    void IFiberAwaiter::EnableSymmetricContextSwitch() {
        symmetric_switch_ = true;
    }

}  // namespace NDummyConcurrency::NSynchronization
