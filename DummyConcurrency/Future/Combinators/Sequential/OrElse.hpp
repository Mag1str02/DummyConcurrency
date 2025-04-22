#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T, typename F>
    TryFuture<T> OrElse(TryFuture<T>&& future, F&& user) {
        auto* contract = NState::ContractState<Result<T>>::Create();
        std::move(future).Consume(
            [contract, user = std::move(user)](Result<T>&& result) {
                if (result) {
                    contract->SetValue(std::move(result));
                } else {
                    user(result.error()).Consume([contract](Result<T>&& result) { contract->SetValue(std::move(result)); }, NRuntime::Inline());
                }
            },
            future.GetScheduler());
        return TryFuture<T>(contract, future.GetScheduler());
    }


}  // namespace NDummyConcurrency::NFuture::NCombinators
