#pragma once

#include <DummyConcurrency/Future/Future.hpp>

#include <DummyConcurrency/Future/Combine/Sequential/Result/As.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>

#include <DummyConcurrency/Future/State/FirstContract.hpp>
#include <DummyConcurrency/Future/State/TryFirstContract.hpp>

#include <utility>  // std::move

namespace DummyConcurrency::Future {

    template <typename T>
    TryFuture<T> TryFirst(TryFuture<T> a, TryFuture<T> b) {
        auto* contract = State::TryFirstContract<T>::Create(2);
        std::move(a).Consume([contract](Result<T> result) { contract->SetResult(std::move(result)); }, Scheduler::Inline());
        std::move(b).Consume([contract](Result<T> result) { contract->SetResult(std::move(result)); }, Scheduler::Inline());
        return TryFuture<T>(contract, Scheduler::Inline());
    }

    template <typename T>
    Future<T> PlainFirst(Future<T> a, Future<T> b) {
        static_assert(!::DummyConcurrency::Traits::IsResult<T>);
        auto* contract = State::PlainFirstContract<T>::Create(2);
        std::move(a).Consume([contract](T value) { contract->SetValue(std::move(value)); }, Scheduler::Inline());
        std::move(b).Consume([contract](T value) { contract->SetValue(std::move(value)); }, Scheduler::Inline());
        return Future<T>(contract, Scheduler::Inline());
    }

    template <typename A, typename B>
    auto First(Future<A> f, Future<B> g) {
        if constexpr (::DummyConcurrency::Traits::IsResult<A> || ::DummyConcurrency::Traits::IsResult<B>) {
            return TryFirst(AsTryFuture(std::move(f)), AsTryFuture(std::move(g)));
        } else {
            return PlainFirst(std::move(f), std::move(g));
        }
    }

}  // namespace DummyConcurrency::Future
