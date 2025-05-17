#include "Suspend.hpp"

#include "DummyConcurrency/Fiber/Core/Fiber.hpp"

namespace NDummyConcurrency::NFiber {

    void Suspend(IAwaiter& awaiter) {
        DC_PROFILE_SCOPE();
        Fiber* current_fiber = Fiber::Self();
        DC_ASSERT(current_fiber != nullptr, "fiber::Suspend must be called from fiber");

        current_fiber->Suspend(awaiter);
    }

}  // namespace NDummyConcurrency::NFiber
