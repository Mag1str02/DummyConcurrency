#pragma once

#include "Result.hpp"
#include "Traits.hpp"

#include <DummyConcurrency/Utils/Traits.hpp>

template <typename T, typename C>
auto operator|(DummyConcurrency::Result<T> r, C c) {
    return c.Pipe(std::move(r));
}

namespace DummyConcurrency {

    namespace Combinators {

        template <typename F>
        struct [[nodiscard]] AndThen : public NonCopyable {
            F User;

            explicit AndThen(F u) : User(std::move(u)) {}

            template <typename T>
            using U = Traits::ValueOf<std::invoke_result_t<F, T>>;

            template <typename T>
            Result<U<T>> Pipe(Result<T> r) {
                return std::move(r).and_then(User);
            }
        };

        template <typename F>
        struct [[nodiscard]] Map : public NonCopyable {
            F User;

            explicit Map(F u) : User(std::move(u)) {}

            template <typename T>
            using U = std::invoke_result_t<F, T>;

            template <typename T>
            Result<U<T>> Pipe(Result<T> r) {
                return std::move(r).transform(User);
            }
        };

        template <typename F>
        struct [[nodiscard]] OrElse : public NonCopyable {
            F User;

            explicit OrElse(F u) : User(std::move(u)) {}

            template <typename T>
            Result<T> Pipe(Result<T> r) {
                return std::move(r).or_else(User);
            }
        };

    }  // namespace Combinators

    template <typename F>
    auto AndThen(F user) {
        return Combinators::AndThen{std::move(user)};
    }

    template <typename F>
    auto OrElse(F user) {
        return Combinators::OrElse{std::move(user)};
    }

    template <typename F>
    auto Map(F user) {
        return Combinators::Map{std::move(user)};
    }

}  // namespace DummyConcurrency