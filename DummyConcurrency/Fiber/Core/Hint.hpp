#pragma once

#include <DummyConcurrency/Fiber/Stack/InlineStackPool.hpp>

namespace NDummyConcurrency::NFiber {

    struct Hint {
        IStackProvider* Pool = InlineStackPool();
    };

}  // namespace NDummyConcurrency::NFiber
