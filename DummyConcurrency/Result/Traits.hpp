#pragma once

#include "Result.hpp"

namespace NDummyConcurrency::NResult {

    template <typename T>
    struct Traits {
        static constexpr bool IsResult = false;  // NOLINT
    };
    template <typename T>
    struct Traits<Result<T>> {
        static constexpr bool IsResult = true;  // NOLINT
        using ValueType                = T;
    };

    template <typename T>
    concept CResult = Traits<T>::IsResult;
    template <typename T>
    concept CNotResult = !Traits<T>::IsResult;

}  // namespace NDummyConcurrency::NResult
