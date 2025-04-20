#pragma once

#include "Result.hpp"

namespace DummyConcurrency {

    template <typename T>
    Result<T> ResultOk(T value) {
        return {std::move(value)};
    }
    Status ResultOk() {
        return {Unit{}};
    }
    inline auto ResultError(Error error) {
        return std::unexpected(error);
    }

}  // namespace DummyConcurrency
