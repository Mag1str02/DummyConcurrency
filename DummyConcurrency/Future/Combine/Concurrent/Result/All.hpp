#pragma once

#include <DummyConcurrency/Future/Type/Result.hpp>

#include <tuple>

namespace DummyConcurrency::Future {

    template <typename A, typename B>
    TryFuture<std::tuple<A, B>> BothOk(TryFuture<A>, TryFuture<B>) {
        std::abort();  // Not implemented
    }

}  // namespace DummyConcurrency::Future
