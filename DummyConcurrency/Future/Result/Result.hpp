#pragma once

#include "Error.hpp"

#include <expected>
#include <variant>

namespace DummyConcurrency {

    using Unit = std::monostate;

    template <typename T>
    using Result = std::expected<T, Error>;
    using Status = Result<Unit>;

}  // namespace DummyConcurrency
