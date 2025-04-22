#pragma once

#include "DummyConcurrency/Fiber/Core/Awaiter.hpp"

namespace NDummyConcurrency::NFiber {

    void Suspend(IAwaiter& awaiter);

}  // namespace NDummyConcurrency::NFiber
