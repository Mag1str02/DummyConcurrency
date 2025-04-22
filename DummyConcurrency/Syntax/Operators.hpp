#pragma once

#include <DummyConcurrency/Future/Combinators/Concurrent/All.hpp>
#include <DummyConcurrency/Future/Combinators/Concurrent/First.hpp>

template <typename T, typename C>
auto operator|(NDummyConcurrency::NFuture::Future<T> f, C c) {
    return c.Pipe(std::move(f));
}

template <typename T>
NDummyConcurrency::NFuture::Future<T> operator||(NDummyConcurrency::NFuture::Future<T> lhs, NDummyConcurrency::NFuture::Future<T> rhs) {
    return NDummyConcurrency::NFuture::NCombinators::First(std::move(lhs), std::move(rhs));
}

template <typename A, typename B>
auto operator+(NDummyConcurrency::NFuture::Future<A> lhs, NDummyConcurrency::NFuture::Future<B> rhs) {
    return NDummyConcurrency::NFuture::NCombinators::Both(std::move(lhs), std::move(rhs));
}

template <typename T, typename C>
auto operator|(NDummyConcurrency::NResult::Result<T> r, C c) {
    return c.Pipe(std::move(r));
}