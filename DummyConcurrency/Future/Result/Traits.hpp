#pragma once

#include "Result.hpp"

namespace DummyConcurrency::Traits {

    namespace Detail {

        template <typename T>
        struct ResultTraits {
            static constexpr bool IsResult = false;  // NOLINT
        };

        template <typename T>
        struct ResultTraits<Result<T>> {
            static constexpr bool IsResult = true;  // NOLINT
            using Type                     = T;
        };

    }  // namespace Detail

    template <typename T>
    static constexpr bool IsResult = Detail::ResultTraits<T>::IsResult;  // NOLINT

    template <typename T>
    concept CResult = IsResult<T>;

    template <CResult Result>
    using ValueOf = typename Detail::ResultTraits<Result>::Type;

}  // namespace DummyConcurrency::Traits
