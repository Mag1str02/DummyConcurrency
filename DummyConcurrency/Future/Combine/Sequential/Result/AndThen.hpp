#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Result/Make.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace DummyConcurrency::Future {

    namespace Combinators {

        template <typename F>
        struct [[nodiscard]] AndThen {
            F User;

            explicit AndThen(F u) : User(std::move(u)) {}

            // Non-copyable
            AndThen(const AndThen&) = delete;

            template <typename T>
            using U = ::DummyConcurrency::Traits::ValueOf<Traits::ValueOf<std::invoke_result_t<F, T>>>;

            template <typename T>
            TryFuture<U<T>> Pipe(TryFuture<T> future) {
                auto* contract = State::ContractState<Result<U<T>>>::Create();
                std::move(future).Consume(
                    [contract, func = std::move(User)](Result<T> result) {
                        if (result) {
                            func(std::move(result.value()))
                                .Consume([contract](Result<U<T>> result) { contract->SetValue(result); }, Scheduler::Inline());
                        } else {
                            contract->SetValue(ResultError(result.error()));
                        }
                    },
                    future.GetScheduler());
                return TryFuture<U<T>>(contract, future.GetScheduler());
            }
        };

    }  // namespace Combinators

    /*
     * Asynchronous try-catch
     *
     * TryFuture<T> -> (T -> TryFuture<U>) -> TryFuture<U>
     *
     */

    template <typename F>
    auto AndThen(F user) {
        return Combinators::AndThen{std::move(user)};
    }

}  // namespace DummyConcurrency::Future
