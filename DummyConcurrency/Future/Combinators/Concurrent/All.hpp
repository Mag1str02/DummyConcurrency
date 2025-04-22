#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/BothContract.hpp>
#include <DummyConcurrency/Result/Traits.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>

#include <tuple>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename A, typename B>
    auto Both(Future<A> a, Future<B> b) {
        static_assert(!::NDummyConcurrency::NResult::NTraits::IsResult<A>);
        static_assert(!::NDummyConcurrency::NResult::NTraits::IsResult<B>);

        auto* contract = NState::BothContract<A, B>::Create();
        std::move(a).Consume([contract](A value) { contract->SetValueA(std::move(value)); }, NRuntime::Inline());
        std::move(b).Consume([contract](B value) { contract->SetValueB(std::move(value)); }, NRuntime::Inline());
        return Future<std::tuple<A, B>>(contract, NRuntime::Inline());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
