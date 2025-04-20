#pragma once

#include <DummyConcurrency/Future/Future.hpp>

#include <DummyConcurrency/Future/Combine/Sequential/Flatten.hpp>
#include <DummyConcurrency/Future/Combine/Sequential/Map.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace DummyConcurrency::Future {

    namespace Combinators {

        template <typename F>
        struct [[nodiscard]] FlatMap {
            F User;

            explicit FlatMap(F u) : User(std::move(u)) {}

            // Non-copyable
            FlatMap(const FlatMap&) = delete;

            template <typename T>
            using U = Traits::ValueOf<std::invoke_result_t<F, T>>;

            template <typename T>
            Future<U<T>> Pipe(Future<T> future) {
                auto map     = Map(std::move(User));
                auto ff      = map.Pipe(std::move(future));
                auto flatten = Flatten();
                return flatten.Pipe(std::move(ff));
            }
        };

    }  // namespace Combinators

    /*
     * Compose two asynchronous actions sequentially
     *
     * Monadic bind
     * https://wiki.haskell.org/Typeclassopedia
     *
     * Future<T> -> (T -> Future<U>) -> Future<U>
     *
     */

    template <typename F>
    auto FlatMap(F user) {
        return Combinators::FlatMap{std::move(user)};
    }

}  // namespace DummyConcurrency::Future
