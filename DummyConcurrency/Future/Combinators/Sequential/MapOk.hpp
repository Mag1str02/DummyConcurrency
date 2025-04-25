#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Traits.hpp>
#include <DummyConcurrency/Result/Make.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T, typename F>
    TryFuture<std::invoke_result_t<F, T>> MapOk(TryFuture<T>&& future, F&& user)
        requires CFutureConsumer<F, T>
    {
        auto* contract = NState::ContractState<Result<std::invoke_result_t<F, T>>>::Create();
        std::move(future).Consume(
            [contract, func = std::move(user)](Result<T>&& result) {
                if (result) {
                    contract->SetValue(NResult::Ok(func(std::move(result.value()))));
                } else {
                    contract->SetValue(NResult::Failure(result.error()));
                }
            },
            future.GetScheduler());
        return TryFuture<std::invoke_result_t<F, T>>(contract, future.GetScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
