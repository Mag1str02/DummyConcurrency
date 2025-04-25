#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/BothContract.hpp>
#include <DummyConcurrency/Result/Traits.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>

#include <tuple>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename A, typename B>
    auto Both(Future<A> a, Future<B> b) {
        static_assert(!::NDummyConcurrency::NResult::Traits<A>::IsResult);
        static_assert(!::NDummyConcurrency::NResult::Traits<B>::IsResult);

        auto* contract = NState::BothContract<A, B>::Create();
        std::move(a).Consume([contract](A value) { contract->SetValueA(std::move(value)); }, NRuntime::InlineScheduler());
        std::move(b).Consume([contract](B value) { contract->SetValueB(std::move(value)); }, NRuntime::InlineScheduler());
        return Future<std::tuple<A, B>>(contract, NRuntime::InlineScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
