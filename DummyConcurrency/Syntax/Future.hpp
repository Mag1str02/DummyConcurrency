#pragma once

#include <DummyConcurrency/Future/Combinators/Concurrent/AllOk.hpp>
#include <DummyConcurrency/Future/Combinators/Concurrent/FirstOk.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/AndThen.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/AsTryFuture.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/FlatMap.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/Flatten.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/Map.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/MapOk.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/OrElse.hpp>
#include <DummyConcurrency/Future/Combinators/Sequential/Via.hpp>
#include <DummyConcurrency/Future/CopyFuture.hpp>
#include <DummyConcurrency/Future/Terminators.hpp>

namespace NDummyConcurrency::NFuture::NSyntax {

    using NCombinators::BothOk;
    using NCombinators::FirstOk;

    namespace Detail {
        template <typename F, typename T>
        concept CAndThen = requires(F&& f, T&& t) {
            { f(std::move(t)) } -> CTryFuture;
        };
    }  // namespace Detail

    namespace NPipes {
        template <typename F>
        class [[nodiscard]] AndThen : public NonCopyable {
        public:
            explicit AndThen(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(TryFuture<T>&& future)
                requires Detail::CAndThen<F, T>
            {
                return NCombinators::AndThen(std::move(future), std::move(user_));
            }

        private:
            F user_;
        };

        template <typename F>
        class [[nodiscard]] Map : public NonCopyable {
        public:
            explicit Map(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(Future<T>&& future) {
                return NCombinators::Map(std::move(future), std::move(user_));
            }

        private:
            F user_;
        };

        template <typename F>
        struct [[nodiscard]] FlatMap : public NonCopyable {
        public:
            explicit FlatMap(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(Future<T>&& future) {
                return NCombinators::FlatMap(std::move(future), std::move(user_));
            }

        private:
            F user_;
        };

        template <typename F>
        class [[nodiscard]] MapOk : public NonCopyable {
        public:
            explicit MapOk(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(TryFuture<T>&& future) {
                return NCombinators::MapOk(std::move(future), std::move(user_));
            }

        private:
            F user_;
        };

        template <typename F>
        class [[nodiscard]] OrElse : public NonCopyable {
        public:
            explicit OrElse(F&& u) : user_(std::move(u)) {}

            template <typename T>
            auto Pipe(TryFuture<T>&& future) {
                return NCombinators::OrElse(std::move(future), std::move(user_));
            }

        private:
            F user_;
        };

        class [[nodiscard]] Via {
        public:
            explicit Via(NRuntime::IScheduler& s) : scheduler_(s) {}

            template <typename T>
            Future<T> Pipe(Future<T>&& future) {
                return NCombinators::Via(std::move(future), scheduler_);
            }

        private:
            NRuntime::IScheduler& scheduler_;
        };
        struct [[nodiscard]] AsTryFuture {
            template <typename T>
            auto Pipe(Future<T>&& f) {
                return NCombinators::AsTryFuture(std::move(f));
            }
        };

        struct [[nodiscard]] Flatten {
            template <typename T>
            Future<T> Pipe(TryFuture<Future<T>>&& future) {
                return NCombinators::Flatten(std::move(future));
            }
            template <typename T>
            Future<T> Pipe(Future<Future<T>>&& future) {
                return NCombinators::Flatten(std::move(future));
            }
        };

        struct [[nodiscard]] Get {
            template <typename T>
            T Pipe(Future<T>&& future) {
                return NFuture::Get(std::move(future));
            }
        };
        struct [[nodiscard]] GetOk {
            template <typename T>
            T Pipe(TryFuture<T>&& future) {
                auto result = NFuture::Get(std::move(future));
                if (!result.has_value()) {
                    throw std::runtime_error("Result is not ok");
                }
                return std::move(result.value());
            }
        };

        struct [[nodiscard]] Detach {
            template <typename T>
            void Pipe(Future<T> future) {
                NFuture::Detach(std::move(future));
            }
        };

        struct [[nodiscard]] Copy {
            template <typename T>
            CopyFuture<T> Pipe(Future<T> future) {
                return CopyFuture<T>(std::move(future));
            }
        };

    }  // namespace NPipes

    // Future<T> -> (T -> U) -> Future<U>
    template <typename F>
    auto Map(F&& user) {
        return NPipes::Map(std::move(user));
    }
    // Future<T> -> (T -> Future<U>) -> Future<U>
    template <typename F>
    auto FlatMap(F&& user) {
        return NPipes::FlatMap(std::move(user));
    }
    // TryFuture<T> -> (T -> TryFuture<U>) -> TryFuture<U>
    template <typename F>
    auto AndThen(F&& user) {
        return NPipes::AndThen(std::move(user));
    }
    // TryFuture<T> -> (T -> U) -> TryFuture<U>
    template <typename F>
    auto MapOk(F&& user) {
        return NPipes::MapOk(std::move(user));
    }
    // TryFuture<T> -> (Error -> TryFuture<T>) -> TryFuture<T>
    template <typename F>
    auto OrElse(F&& user) {
        return NPipes::OrElse{std::move(user)};
    }

    /*
     * Future<T> -> TryFuture<T'> where
     *    T' = U for T = Result<U>
     *    T' = T otherwise
     */
    inline auto AsTryFuture() {
        return NPipes::AsTryFuture();
    }
    // Future<Future<T>> -> Future<T>
    inline auto Flatten() {
        return NPipes::Flatten();
    }
    // Future<T> -> Scheduler -> Future<T>
    inline auto Via(NRuntime::IScheduler& scheduler) {
        return NPipes::Via(scheduler);
    }
    inline auto Copy() {
        return NPipes::Copy();
    }

    // Future<T> -> T
    inline auto Get() {
        return NPipes::Get{};
    }
    // TryFuture<T> -> T
    inline auto GetOk() {
        return NPipes::GetOk{};
    }
    // Future<T> -> (straight to hell)
    inline auto Detach() {
        return NPipes::Detach{};
    }

}  // namespace NDummyConcurrency::NFuture::NSyntax