#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/TryBothContract.hpp>
#include <DummyConcurrency/Scheduler/Inline.hpp>

#include <tuple>

namespace DummyConcurrency::Future {

    template <typename A, typename B>
    TryFuture<std::tuple<A, B>> BothOk(TryFuture<A> a, TryFuture<B> b) {
        auto* contract = State::TryBothContract<A, B>::Create();
        std::move(a).Consume([contract](Result<A> value) { contract->SetResultA(std::move(value)); }, Scheduler::Inline());
        std::move(b).Consume([contract](Result<B> value) { contract->SetResultB(std::move(value)); }, Scheduler::Inline());
        return TryFuture<std::tuple<A, B>>(contract, Scheduler::Inline());
    }

}  // namespace DummyConcurrency::Future
