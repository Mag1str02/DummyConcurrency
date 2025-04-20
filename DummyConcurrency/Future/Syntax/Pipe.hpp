#pragma once

#include <DummyConcurrency/Future/Future.hpp>

#include <utility>  // std::move

/*
 * Pipeline operator for sequential Future composition
 *
 * Usage:
 *
 * auto f = future::Value(7)
 *          | future::Map([](int v) { return v + 1; });
 *
 */

template <typename T, typename C>
auto operator|(DummyConcurrency::Future::Future<T> f, C c) {
    return c.Pipe(std::move(f));
}
