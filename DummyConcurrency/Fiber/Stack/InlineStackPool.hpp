#pragma once

#include "IStackPool.hpp"

namespace DummyConcurrency::Fiber {

    IStackPool* InlineStackPool();

}  // namespace DummyConcurrency::Fiber
