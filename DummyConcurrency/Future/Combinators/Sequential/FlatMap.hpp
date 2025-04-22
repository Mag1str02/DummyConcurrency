#pragma once

#include <DummyConcurrency/Future/Combinators/Sequential/Flatten.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/Map.hpp>
#include <DummyConcurrency/Future/Future.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T, typename F>
    auto FlatMap(Future<T>&& future, F&& user) {
        return Flatten(Map(std::move(future), std::move(user)));
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
