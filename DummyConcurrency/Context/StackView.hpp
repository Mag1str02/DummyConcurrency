#pragma once

#include <cstddef>

namespace DummyConcurrency::Context {

    struct StackView {
        std::byte* Bottom;
        std::byte* Top;
    };

}  // namespace DummyConcurrency::Context
