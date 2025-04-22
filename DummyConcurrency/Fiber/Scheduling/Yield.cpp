#include "Yield.hpp"

#include "DummyConcurrency/Fiber/Core/Awaiter.hpp"
#include "DummyConcurrency/Fiber/Core/Fiber.hpp"

namespace NDummyConcurrency::NFiber {

    struct YieldAwaiter : public IAwaiter {
        virtual void OnSuspend() noexcept override { Schedule(); }
    };

    void Yield() {
        Fiber* current_fiber = Fiber::Self();
        DC_ASSERT(current_fiber != nullptr, "fiber::Yield must be called from fiber");

        YieldAwaiter awaiter;
        current_fiber->Suspend(awaiter);
    }

}  // namespace NDummyConcurrency::NFiber
