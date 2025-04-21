#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>

#include <cstdlib>  // std::abort

namespace DummyConcurrency::Future {

    namespace Combinators {

        struct [[nodiscard]] Flatten {
            Flatten() = default;

            // Non-copyable
            Flatten(const Flatten&) = delete;

            template <typename T>
            Future<T> Pipe(Future<Future<T>> future) {
                auto* contract = State::ContractState<T>::Create();
                std::move(future).Consume(
                    [contract](Future<T> value) {
                        std::move(value).Consume([contract](T value) { contract->SetValue(std::move(value)); }, Runtime::Inline());
                    },
                    Runtime::Inline());
                return Future<T>(contract, future.GetScheduler());
            }
        };

    }  // namespace Combinators

    /*
     * Collapse nested Future-s
     *
     * Future<Future<T>> -> Future<T>
     *
     */

    inline auto Flatten() {
        return Combinators::Flatten{};
    }

}  // namespace DummyConcurrency::Future
