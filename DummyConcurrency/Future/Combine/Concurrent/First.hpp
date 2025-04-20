#pragma once

#include <DummyConcurrency/Future/Combine/Sequential/Result/AsTryFuture.hpp>
#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>
#include <DummyConcurrency/Future/State/FirstContract.hpp>

#include <utility>  // std::move

namespace DummyConcurrency::Future {

    template <typename T>
    auto First(Future<T> a, Future<T> b) {
        static_assert(!::DummyConcurrency::Traits::IsResult<T>);
        auto* contract = State::PlainFirstContract<T>::Create(2);
        std::move(a).Consume([contract](T value) { contract->SetValue(std::move(value)); }, Scheduler::Inline());
        std::move(b).Consume([contract](T value) { contract->SetValue(std::move(value)); }, Scheduler::Inline());
        return Future<T>(contract, Scheduler::Inline());
    }

}  // namespace DummyConcurrency::Future
