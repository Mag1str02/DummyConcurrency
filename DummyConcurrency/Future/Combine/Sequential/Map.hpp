#pragma once

#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Syntax/Pipe.hpp>

#include <type_traits>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace DummyConcurrency::Future {

    namespace Combinators {

        template <typename F>
        struct [[nodiscard]] Map {
            F User;

            explicit Map(F u) : User(std::move(u)) {}

            // Non-copyable
            Map(const Map&) = delete;

            template <typename T>
            using U = std::invoke_result_t<F, T>;

            template <typename T>
            Future<U<T>> Pipe(Future<T> future) {
                auto* contract = State::ContractState<U<T>>::Create();
                std::move(future).Consume([func = std::move(User), contract](T value) { contract->SetValue(func(std::move(value))); },
                                          future.GetScheduler());
                return Future<U<T>>(contract, future.GetScheduler());
            }
        };

    }  // namespace Combinators

    /*
     * Functor
     * https://wiki.haskell.org/Typeclassopedia
     *
     * Future<T> -> (T -> U) -> Future<U>
     *
     */

    template <typename F>
    auto Map(F user) {
        return Combinators::Map{std::move(user)};
    }

}  // namespace DummyConcurrency::Future
