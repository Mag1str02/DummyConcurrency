#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>

#include <DummyConcurrency/Future/Result/Make.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace DummyConcurrency::Future {

    namespace Combinators {

        template <typename F>
        struct [[nodiscard]] MapOk {
            F User;

            explicit MapOk(F u) : User(std::move(u)) {}

            // Non-copyable
            MapOk(const MapOk&) = delete;

            template <typename T>
            using U = std::invoke_result_t<F, T>;

            template <typename T>
            TryFuture<U<T>> Pipe(TryFuture<T> future) {
                auto* contract = State::ContractState<Result<U<T>>>::Create();
                std::move(future).Consume(
                    [contract, func = std::move(User)](Result<T> result) {
                        if (result) {
                            contract->SetValue(ResultOk(func(std::move(result.value()))));
                        } else {
                            contract->SetValue(ResultError(result.error()));
                        }
                    },
                    future.GetScheduler());
                return TryFuture<U<T>>(contract, future.GetScheduler());
            }
        };

    }  // namespace Combinators

    // TryFuture<T> -> (T -> U) -> TryFuture<U>

    template <typename F>
    auto MapOk(F user) {
        return Combinators::MapOk(std::move(user));
    }

}  // namespace DummyConcurrency::Future
