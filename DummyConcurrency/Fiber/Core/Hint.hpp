#pragma once

#include <DummyConcurrency/Fiber/Stack/InlineStackPool.hpp>

namespace DummyConcurrency::Fiber {

    struct Hint {
        IStackPool* Pool = InlineStackPool();
    };

}  // namespace DummyConcurrency::Fiber
