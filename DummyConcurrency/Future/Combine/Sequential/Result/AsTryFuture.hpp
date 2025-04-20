#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Result/Make.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>

#include <DummyConcurrency/Future/Combine/Sequential/Map.hpp>

#include <type_traits>

namespace DummyConcurrency::Future {

    // Explicit

    /*
     * Future<T> -> TryFuture<T'> where
     *    T' = U for T = Result<U>
     *    T' = T otherwise
     */

    template <typename T>
    TryFuture<T> AsTryFuture(TryFuture<T> f) {
        return std::move(f);  // Identity
    }

    template <typename T, std::enable_if_t<!::DummyConcurrency::Traits::IsResult<T>, bool> = true>
    TryFuture<T> AsTryFuture(Future<T> f) {
        return std::move(f) | Map([](T v) { return ResultOk(v); });
    }

    // Chaining

    namespace Combinators {

        struct [[nodiscard]] AsTryFuture {
            AsTryFuture() = default;

            // Non-copyable
            AsTryFuture(const AsTryFuture&) = delete;

            template <typename T>
            auto Pipe(Future<T> f) {
                return ::DummyConcurrency::Future::AsTryFuture(std::move(f));
            }
        };

    }  // namespace Combinators

    auto AsTryFuture() {
        return Combinators::AsTryFuture{};
    }

}  // namespace DummyConcurrency::Future
