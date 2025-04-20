#pragma once

#include <DummyConcurrency/Future/Combine/Concurrent/All.hpp>

/*
 * Syntactic sugar for future::Both combinator
 *
 * Usage:
 *
 * auto both = std::move(lhs) + std::move(rhs);
 *
 */

template <typename A, typename B>
auto operator+(DummyConcurrency::Future::Future<A> lhs, DummyConcurrency::Future::Future<B> rhs) {
    return DummyConcurrency::Future::Both(std::move(lhs), std::move(rhs));
}
