#pragma once

#include <DummyConcurrency/Future/Combinators/Sequential/Flatten.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/Map.hpp>
#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Traits.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T, typename F>
    auto FlatMap(Future<T>&& future, F&& user)
        requires CFutureConsumer<F, T>
    {
        return Flatten(Map(std::move(future), std::move(user)));
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
