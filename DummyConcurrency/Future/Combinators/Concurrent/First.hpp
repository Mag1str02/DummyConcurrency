#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/FirstContract.hpp>
#include <DummyConcurrency/Result/Traits.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T>
    auto First(Future<T> a, Future<T> b) {
        static_assert(!::NDummyConcurrency::NResult::NTraits::IsResult<T>);
        auto* contract = NState::PlainFirstContract<T>::Create(2);
        std::move(a).Consume([contract](T value) { contract->SetValue(std::move(value)); }, NRuntime::Inline());
        std::move(b).Consume([contract](T value) { contract->SetValue(std::move(value)); }, NRuntime::Inline());
        return Future<T>(contract, NRuntime::Inline());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
