#pragma once

#include <DummyConcurrency/Future/Combine/Concurrent/First.hpp>

/*
 * Syntactic sugar for future::First combinator
 *
 * Usage:
 *
 * auto first = std::move(lhs) or std::move(rhs);
 *
 */

template <typename T>
DummyConcurrency::Future::Future<T> operator||(DummyConcurrency::Future::Future<T> lhs, DummyConcurrency::Future::Future<T> rhs) {
    return DummyConcurrency::Future::First(std::move(lhs), std::move(rhs));
}
