#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Traits.hpp>
#include <DummyConcurrency/Result/Make.hpp>
#include <DummyConcurrency/Result/Traits.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    namespace Detail {
        template <typename T, typename F>
        using InvokeResult = NResult::Traits<typename Traits<std::invoke_result_t<F, T>>::ValueType>::ValueType;
    }

    template <typename T, typename F>
    TryFuture<Detail::InvokeResult<T, F>> AndThen(TryFuture<T>&& future, F&& user)
        requires CFutureConsumer<F, T>
    {
        auto* contract = NState::ContractState<Result<Detail::InvokeResult<T, F>>>::Create();
        std::move(future).Consume(
            [contract, func = std::move(user)](Result<T>&& result) {
                if (result) {
                    func(std::move(result.value()))
                        .Consume([contract](Result<Detail::InvokeResult<T, F>> result) { contract->SetValue(result); }, NRuntime::InlineScheduler());
                } else {
                    contract->SetValue(NResult::Failure(result.error()));
                }
            },
            future.GetScheduler());
        return TryFuture<Detail::InvokeResult<T, F>>(contract, future.GetScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
