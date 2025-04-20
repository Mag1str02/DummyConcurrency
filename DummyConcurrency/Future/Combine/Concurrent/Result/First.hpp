#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/TryFirstContract.hpp>
#include <DummyConcurrency/Scheduler/Inline.hpp>

namespace DummyConcurrency::Future {

    // First success / last error

    template <typename T>
    TryFuture<T> FirstOk(TryFuture<T> a, TryFuture<T> b) {
        auto* contract = State::TryFirstContract<T>::Create(2);
        std::move(a).Consume([contract](Result<T> result) { contract->SetResult(std::move(result)); }, Scheduler::Inline());
        std::move(b).Consume([contract](Result<T> result) { contract->SetResult(std::move(result)); }, Scheduler::Inline());
        return TryFuture<T>(contract, Scheduler::Inline());
    }

}  // namespace DummyConcurrency::Future
