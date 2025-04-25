#pragma once

#include <DummyConcurrency/Fiber/Stack/InlineStackPool.hpp>

namespace NDummyConcurrency::NFiber {

    struct Hint {
        IStackProvider* StackProvider = InlineStackPool();
    };

}  // namespace NDummyConcurrency::NFiber
