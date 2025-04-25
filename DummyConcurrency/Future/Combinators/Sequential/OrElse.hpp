#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Traits.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T, typename F>
    TryFuture<T> OrElse(TryFuture<T>&& future, F&& user)
        requires CFutureConsumer<F, Error>
    {
        auto* contract = NState::ContractState<Result<T>>::Create();
        std::move(future).Consume(
            [contract, user = std::move(user)](Result<T>&& result) {
                if (result) {
                    contract->SetValue(std::move(result));
                } else {
                    user(result.error()).Consume([contract](Result<T>&& result) { contract->SetValue(std::move(result)); }, NRuntime::InlineScheduler());
                }
            },
            future.GetScheduler());
        return TryFuture<T>(contract, future.GetScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
