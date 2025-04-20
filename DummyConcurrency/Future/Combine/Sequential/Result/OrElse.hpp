#pragma once

#include <DummyConcurrency/Future/Future.hpp>

#include <DummyConcurrency/Future/State/Contract.hpp>

#include <cstdlib>  // std::abort
#include <type_traits>
#include <utility>  // std::move

namespace DummyConcurrency::Future {

    namespace Combinators {

        template <typename F>
        struct [[nodiscard]] OrElse {
            F User;

            explicit OrElse(F u) : User(std::move(u)) {}

            // Non-copyable
            OrElse(const OrElse&) = delete;

            template <typename T>
            TryFuture<T> Pipe(TryFuture<T> future) {
                auto* contract = State::ContractState<Result<T>>::Create();
                std::move(future).Consume(
                    [contract, error_callback = std::move(User)](Result<T> result) {
                        if (result) {
                            contract->SetValue(std::move(result));
                        } else {
                            error_callback(result.error())
                                .Consume([contract](Result<T> result) { contract->SetValue(std::move(result)); }, Scheduler::Inline());
                        }
                    },
                    future.GetScheduler());
                return TryFuture<T>(contract, future.GetScheduler());
            }
        };

    }  // namespace Combinators

    /*
     * Asynchronous try-catch
     *
     * TryFuture<T> -> (Error -> TryFuture<T>) -> TryFuture<T>
     *
     */

    template <typename F>
    auto OrElse(F user) {
        return Combinators::OrElse{std::move(user)};
    }

}  // namespace DummyConcurrency::Future
