#pragma once

#include "Future.hpp"

namespace NDummyConcurrency::NFuture {

    template <typename T>
    struct Traits {
        static constexpr bool IsFuture    = false;  // NOLINT
        static constexpr bool IsTryFuture = false;  // NOLINT
    };

    template <typename T>
    struct Traits<TryFuture<T>> {
        static constexpr bool IsFuture    = true;  // NOLINT
        static constexpr bool IsTryFuture = true;  // NOLINT
        using ValueType                   = Result<T>;
        using ActualType                  = T;
    };

    template <typename T>
    struct Traits<Future<T>> {
        static constexpr bool IsFuture    = true;   // NOLINT
        static constexpr bool IsTryFuture = false;  // NOLINT
        using ValueType                   = T;
        using ActualType                  = T;
    };

    template <typename T>
    concept CFuture = Traits<T>::IsFuture && std::is_move_constructible_v<typename Traits<T>::ActualType>;
    template <typename T>
    concept CTryFuture = Traits<T>::IsTryFuture && std::is_move_constructible_v<typename Traits<T>::ActualType>;
    template <typename F, typename T>
    concept CFutureConsumer = requires(T&& t, F&& f) { f(std::move(t)); } && std::move_constructible<T>;

}  // namespace NDummyConcurrency::NFuture