#pragma once

#include "Result.hpp"

namespace NDummyConcurrency::NResult {

    template <typename T>
    Result<T> Ok(T value) {
        return {std::move(value)};
    }
    inline Status Ok() {
        return {Unit{}};
    }
    inline auto Failure(Error error) {
        return std::unexpected(error);
    }

}  // namespace NDummyConcurrency::NResult
