#pragma once

#include <DummyConcurrency/Fiber/Stack/InlineStackPool.hpp>

namespace NDummyConcurrency::NFiber {

    struct Hint {
        IStackPool* Pool = InlineStackPool();
    };

}  // namespace NDummyConcurrency::NFiber
