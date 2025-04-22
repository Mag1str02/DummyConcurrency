#pragma once

#include <DummyConcurrency/Future/Future.hpp>

namespace NDummyConcurrency::NFuture::NCombinators {

    template <typename T>
    Future<T> Via(Future<T>&& future, NRuntime::IScheduler& scheduler) {
        future.SetScheduler(scheduler);
        return future;
    }

}  // namespace NDummyConcurrency::NFuture::NCombinators
