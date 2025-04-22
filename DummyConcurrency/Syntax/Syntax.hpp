#pragma once

#include "Future.hpp"     // IWYU pragma: export
#include "Operators.hpp"  // IWYU pragma: export
#include "Result.hpp"     // IWYU pragma: export

#include <DummyConcurrency/Future/Make.hpp>  // IWYU pragma: export
#include <DummyConcurrency/Result/Make.hpp>  // IWYU pragma: export
#include <DummyConcurrency/Result/Make.hpp>  // IWYU pragma: export

namespace NDummyConcurrency {
    template <typename T>
    using Future = NFuture::Future<T>;
    template <typename T>
    using TryFuture = NFuture::TryFuture<T>;
    template <typename T>
    using Result = NResult::Result<T>;

    using NResult::Error;
    using NResult::Status;
    using NResult::Unit;

}  // namespace NDummyConcurrency