#include "Suspend.hpp"

#include "DummyConcurrency/Fiber/Core/Fiber.hpp"

namespace DummyConcurrency::Fiber {

    void Suspend(IAwaiter& awaiter) {
        Fiber* current_fiber = Fiber::Self();
        DC_ASSERT(current_fiber != nullptr, "fiber::Suspend must be called from fiber");

        current_fiber->Suspend(awaiter);
    }

}  // namespace DummyConcurrency::Fiber
