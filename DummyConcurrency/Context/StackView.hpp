#pragma once

#include <cstddef>

namespace NDummyConcurrency::NContext {

    struct StackView {
        std::byte* Bottom;
        std::byte* Top;
    };

}  // namespace NDummyConcurrency::NContext
