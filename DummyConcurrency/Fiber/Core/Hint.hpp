#pragma once

#include <DummyConcurrency/Fiber/Stack/InlineStackPool.hpp>

namespace NDummyConcurrency::NFiber {

    struct Hint {
        IStackProvider* StackProvider = InlineStackPool();
        const char*     Name          = nullptr;
        int32_t         Group         = 0;
    };

}  // namespace NDummyConcurrency::NFiber
