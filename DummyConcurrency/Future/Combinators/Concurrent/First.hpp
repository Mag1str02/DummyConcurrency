#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/FirstContract.hpp>
#include <DummyConcurrency/Result/Traits.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T>
    auto First(Future<T> a, Future<T> b) {
        static_assert(!::NDummyConcurrency::NResult::Traits<T>::IsResult);
        auto* contract = NState::PlainFirstContract<T>::Create(2);
        std::move(a).Consume([contract](T value) { contract->SetValue(std::move(value)); }, NRuntime::InlineScheduler());
        std::move(b).Consume([contract](T value) { contract->SetValue(std::move(value)); }, NRuntime::InlineScheduler());
        return Future<T>(contract, NRuntime::InlineScheduler());
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
