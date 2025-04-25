#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/TryFirstContract.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    // First success / last error

    template <typename T>
    TryFuture<T> FirstOk(TryFuture<T> a, TryFuture<T> b) {
        auto* contract = NState::TryFirstContract<T>::Create(2);
        std::move(a).Consume([contract](Result<T> result) { contract->SetResult(std::move(result)); }, NRuntime::InlineScheduler());
        std::move(b).Consume([contract](Result<T> result) { contract->SetResult(std::move(result)); }, NRuntime::InlineScheduler());
        return TryFuture<T>(contract, NRuntime::InlineScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
