#pragma once

#include <DummyConcurrency/Future/Type/Result.hpp>

namespace DummyConcurrency::Future {

    // First success / last error

    template <typename T>
    TryFuture<T> FirstOk(TryFuture<T>, TryFuture<T>) {
        std::abort();  // Not implemented
    }

}  // namespace DummyConcurrency::Future
