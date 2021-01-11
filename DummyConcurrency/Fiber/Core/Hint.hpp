#pragma once

#include <cstdint>

namespace DummyConcurrency::Fiber {

    struct Hint {
        uint64_t StackeSize = 1 * 1024 * 1024;
    };

}  // namespace DummyConcurrency::Fiber
