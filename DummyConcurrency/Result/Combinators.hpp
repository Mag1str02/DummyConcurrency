#pragma once

#include "Result.hpp"
#include "Traits.hpp"

namespace NDummyConcurrency::NResult::NCombinators {

    template <typename T, typename F>
    Result<NTraits::ValueOf<std::invoke_result_t<F, T>>> AndThen(Result<T>&& result, F&& user) {
        return std::move(result).and_then(user);
    }
    template <typename T, typename F>
    Result<std::invoke_result_t<F, T>> Map(Result<T>&& result, F&& user) {
        return std::move(result).transform(user);
    }
    template <typename T, typename F>
    Result<T> OrElse(Result<T>&& result, F&& user) {
        return std::move(result).or_else(user);
    }

}  // namespace NDummyConcurrency::NResult::NCombinators
