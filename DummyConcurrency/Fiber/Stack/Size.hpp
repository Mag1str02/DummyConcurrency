#pragma once

#include <cstdint>

namespace DummyConcurrency::Fiber {

    enum class StackSize {
        Tiny,    // 8KB,
        Small,   // 256KB
        Medium,  // 1MB
        Large,   // 8MB

    };

    uint64_t SizeInBytes(StackSize size);

}  // namespace DummyConcurrency::Fiber::Stack
