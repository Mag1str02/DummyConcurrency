#pragma once

#include "DummyConcurrency/Fiber/Core/Awaiter.hpp"

namespace DummyConcurrency::Fiber {

    void Suspend(IAwaiter& awaiter);

}  // namespace DummyConcurrency::Fiber
