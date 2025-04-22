#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T>
    Future<T> Flatten(Future<Future<T>>&& future) {
        auto* contract = NState::ContractState<T>::Create();
        std::move(future).Consume(
            [contract](Future<T> value) {
                std::move(value).Consume([contract](T value) { contract->SetValue(std::move(value)); }, NRuntime::Inline());
            },
            NRuntime::Inline());
        return Future<T>(contract, future.GetScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
