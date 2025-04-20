#pragma once

#include <DummyConcurrency/Future/Future.hpp>

#include <DummyConcurrency/Future/State/BothContract.hpp>

#include <DummyConcurrency/Future/Combine/Sequential/Result/AsTryFuture.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>
#include <DummyConcurrency/Scheduler/Inline.hpp>

#include <tuple>

namespace DummyConcurrency::Future {

    template <typename A, typename B>
    auto Both(Future<A> a, Future<B> b) {
        static_assert(!::DummyConcurrency::Traits::IsResult<A>);
        static_assert(!::DummyConcurrency::Traits::IsResult<B>);

        auto* contract = State::BothContract<A, B>::Create();
        std::move(a).Consume([contract](A value) { contract->SetValueA(std::move(value)); }, Scheduler::Inline());
        std::move(b).Consume([contract](B value) { contract->SetValueB(std::move(value)); }, Scheduler::Inline());
        return Future<std::tuple<A, B>>(contract, Scheduler::Inline());
    }

}  // namespace DummyConcurrency::Future
