#include "Fiber.hpp"

#include <tracy/Tracy.hpp>

#include <DummyConcurrency/Utils/Assert.hpp>

namespace NDummyConcurrency::NImplementationLayer::NDummyEngine {

    static thread_local Fiber* gCurrentFiber = nullptr;

    FiberHandle SwitchToFiber(FiberHandle handle) {
        Fiber* current = gCurrentFiber;
        gCurrentFiber  = handle.fiber_;
        if (handle.fiber_ != nullptr) {
            DC_ASSERT(handle.fiber_->name_ != nullptr, "No name in fiber handle");
            // std::println("Switching to fiber {}", handle.fiber_->name_);
            TracyFiberEnterHint(handle.fiber_->name_, handle.fiber_->group_);
        } else {
            // std::println("Leaving fiber {}", current->name_);
            TracyFiberLeave;
        }
        return FiberHandle(current);
    }

}  // namespace NDummyConcurrency::NImplementationLayer::NDummyEngine