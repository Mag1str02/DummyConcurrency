#pragma once

#include <DummyConcurrency/Result/Combinators.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

namespace NDummyConcurrency::NResult::NSyntax {

    namespace NPipes {

        template <typename F>
        class [[nodiscard]] AndThen : public NonCopyable {
        public:
            explicit AndThen(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(Result<T>&& result) {
                return NCombinators::AndThen(std::move(result), std::move(user_));
            }

        private:
            F user_;
        };

        template <typename F>
        class [[nodiscard]] Map : public NonCopyable {
        public:
            explicit Map(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(Result<T>&& result) {
                return NCombinators::Map(std::move(result), std::move(user_));
            }

        private:
            F user_;
        };

        template <typename F>
        class [[nodiscard]] OrElse : public NonCopyable {
        public:
            explicit OrElse(F&& u) : user_(std::move(u)) {}

            template <typename T>
            Result<T> Pipe(Result<T>&& result) {
                return NCombinators::OrElse(std::move(result), std::move(user_));
            }

        private:
            F user_;
        };

    }  // namespace NPipes

    // Result<T> -> (T -> Result<U>) -> Result<U>
    template <typename F>
    auto AndThen(F&& user) {
        return NPipes::AndThen(std::move(user));
    }
    // Result<T> -> (T -> U) -> Result<U>
    template <typename F>
    auto Map(F&& user) {
        return NPipes::Map(std::move(user));
    }
    // Result<T> -> (Error -> Result<T>) -> Result<T>
    template <typename F>
    auto OrElse(F&& user) {
        return NPipes::OrElse(std::move(user));
    }

}  // namespace NDummyConcurrency::NResult