#pragma once

#include "DummyConcurrency/Fiber/Core/Fiber.hpp"

namespace NDummyConcurrency::NFiber {

    template <typename F>
    void Go(IScheduler& scheduler, F&& body, Hint hint = {}) {
        DC_PROFILE_SCOPE();
        Fiber* fiber = Fiber::Create(scheduler, std::move(body), hint);
        fiber->Schedule();
    }

    template <typename F>
    void Go(F&& body, Hint hint = {}) {
        DC_PROFILE_SCOPE();
        Fiber* current_fiber = Fiber::Self();
        DC_ASSERT(current_fiber != nullptr, "fiber::Go(Body body) must be called from fiber");

        Fiber* fiber = Fiber::Create(current_fiber->GetScheduler(), std::move(body), hint);
        fiber->Schedule();
    }

}  // namespace NDummyConcurrency::NFiber
