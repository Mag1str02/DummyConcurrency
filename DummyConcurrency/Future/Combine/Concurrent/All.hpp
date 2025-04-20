#pragma once

#include <DummyConcurrency/Future/Future.hpp>

#include <DummyConcurrency/Future/State/BothContract.hpp>
#include <DummyConcurrency/Future/State/TryBothContract.hpp>

#include <DummyConcurrency/Future/Combine/Seq/Result/As.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>
#include <DummyConcurrency/Scheduler/Inline.hpp>

#include <tuple>

namespace DummyConcurrency::Future {

    template <typename A, typename B>
    TryFuture<std::tuple<A, B>> TryBoth(TryFuture<A> a, TryFuture<B> b) {
        auto* contract = State::TryBothContract<A, B>::Create();
        std::move(a).Consume([contract](Result<A> value) { contract->SetResultA(std::move(value)); }, Scheduler::Inline());
        std::move(b).Consume([contract](Result<B> value) { contract->SetResultB(std::move(value)); }, Scheduler::Inline());
        return TryFuture<std::tuple<A, B>>(contract, Scheduler::Inline());
    }

    template <typename A, typename B>
    Future<std::tuple<A, B>> PlainBoth(Future<A> a, Future<B> b) {
        static_assert(!::DummyConcurrency::Traits::IsResult<A>);
        static_assert(!::DummyConcurrency::Traits::IsResult<B>);

        auto* contract = state::BothContract<A, B>::Create();
        std::move(a).Consume([contract](A value) { contract->SetValueA(std::move(value)); }, Scheduler::Inline());
        std::move(b).Consume([contract](B value) { contract->SetValueB(std::move(value)); }, Scheduler::Inline());
        return Future<std::tuple<A, B>>(contract, Scheduler::Inline());
    }

    template <typename A, typename B>
    auto Both(Future<A> f, Future<B> g) {
        if constexpr (::DummyConcurrency::Traits::IsResult<A> || ::DummyConcurrency::Traits::IsResult<B>) {
            return TryBoth(AsTryFuture(std::move(f)), AsTryFuture(std::move(g)));
        } else {
            return PlainBoth(std::move(f), std::move(g));
        }
    }

}  // namespace DummyConcurrency::Future
