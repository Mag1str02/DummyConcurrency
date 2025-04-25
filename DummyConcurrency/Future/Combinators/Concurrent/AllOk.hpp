#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/TryBothContract.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>

#include <tuple>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename A, typename B>
    TryFuture<std::tuple<A, B>> BothOk(TryFuture<A> a, TryFuture<B> b) {
        auto* contract = NState::TryBothContract<A, B>::Create();
        std::move(a).Consume([contract](Result<A> value) { contract->SetResultA(std::move(value)); }, NRuntime::InlineScheduler());
        std::move(b).Consume([contract](Result<B> value) { contract->SetResultB(std::move(value)); }, NRuntime::InlineScheduler());
        return TryFuture<std::tuple<A, B>>(contract, NRuntime::InlineScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
