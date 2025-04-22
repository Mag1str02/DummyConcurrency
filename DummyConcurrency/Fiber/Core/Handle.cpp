#include "Handle.hpp"

#include "Fiber.hpp"

#include <DummyConcurrency/Utils/Assert.hpp>

namespace NDummyConcurrency::NFiber {

    Fiber* Handle::Release() {
        DC_ASSERT(IsValid(), "Invalid fiber handle");
        return std::exchange(fiber_, nullptr);
    }

    void Handle::Schedule() {
        Release()->Schedule();
    }

    void Handle::Switch(Handle& current_fiber) {
        Fiber* current = Fiber::Self();
        DC_ASSERT(current != nullptr, "Cannot switch from outside of fiber");

        current_fiber = Handle(current);
        current->SwitchTo(*Release());
    }

}  // namespace NDummyConcurrency::NFiber
