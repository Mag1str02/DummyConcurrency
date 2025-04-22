#pragma once

#include <DummyConcurrency/Future/Combinators/Sequential/Map.hpp>
#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Result/Make.hpp>
#include <DummyConcurrency/Result/Traits.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T>
    TryFuture<T> AsTryFuture(TryFuture<T>&& f) {
        return std::move(f);  // Identity
    }

    template <typename T, std::enable_if_t<!::NDummyConcurrency::NResult::NTraits::IsResult<T>, bool> = true>
    TryFuture<T> AsTryFuture(Future<T>&& f) {
        return Map(std::move(f), [](T&& v) { return NResult::Ok(std::move(v)); });
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
