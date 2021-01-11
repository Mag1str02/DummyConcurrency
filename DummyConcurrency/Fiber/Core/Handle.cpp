#include "Handle.hpp"

#include "Fiber.hpp"

#include <DummyConcurrency/Common.hpp>

namespace DummyConcurrency::Fiber {

    Fiber* FiberHandle::Release() {
        DC_ASSERT(IsValid(), "Invalid fiber handle");
        return std::exchange(fiber_, nullptr);
    }

    void FiberHandle::Schedule() {
        Release()->Schedule();
    }

    void FiberHandle::Switch(FiberHandle& current_fiber) {
        Fiber* current = Fiber::Self();
        DC_ASSERT(current != nullptr, "Cannot switch from outside of fiber");

        current_fiber = FiberHandle(current);
        current->SwitchTo(*Release());
    }

}  // namespace DummyConcurrency::Fiber
