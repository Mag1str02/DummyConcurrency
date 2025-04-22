#pragma once

#include "Error.hpp"

#include <expected>
#include <variant>

namespace NDummyConcurrency::NResult {

    using Unit = std::monostate;

    template <typename T>
    using Result = std::expected<T, Error>;
    using Status = Result<Unit>;

}  // namespace NDummyConcurrency::NResult
