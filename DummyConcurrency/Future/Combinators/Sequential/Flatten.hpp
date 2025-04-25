#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Result/Traits.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T>
    Future<T> Flatten(Future<T>&& future) {
        auto* contract = NState::ContractState<T>::Create();
        std::move(future).Consume(
            [contract](Future<T> value) {
                std::move(value).Consume([contract](T value) { contract->SetValue(std::move(value)); }, NRuntime::InlineScheduler());
            },
            NRuntime::InlineScheduler());
        return Future<T>(contract, future.GetScheduler());
    }

    template <typename T>
    TryFuture<T> Flatten(TryFuture<Future<T>>&& future) {
        auto* contract = NState::ContractState<Result<T>>::Create();
        std::move(future).Consume(
            [contract](Result<Future<T>>&& value) {
                if (value.has_value()) {
                    std::move(value.value()).Consume([contract](T&& result) { contract->SetValue(std::move(result)); }, NRuntime::InlineScheduler());
                } else {
                    contract->SetValue(std::unexpected(value.error()));
                }
            },
            NRuntime::InlineScheduler());
        return TryFuture<T>(contract, future.GetScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
