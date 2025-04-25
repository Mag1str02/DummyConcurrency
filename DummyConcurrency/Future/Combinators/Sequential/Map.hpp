#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Traits.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T, typename F>
    Future<std::invoke_result_t<F, T>> Map(Future<T>&& future, F&& user)
        requires CFutureConsumer<F, T>
    {
        auto* contract = NState::ContractState<std::invoke_result_t<F, T>>::Create();
        std::move(future).Consume([func = std::move(user), contract](T value) { contract->SetValue(func(std::move(value))); }, future.GetScheduler());
        return Future<std::invoke_result_t<F, T>>(contract, future.GetScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
