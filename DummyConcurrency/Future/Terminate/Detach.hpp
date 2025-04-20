#pragma once

#include <DummyConcurrency/Future/Syntax/Pipe.hpp>

#include <DummyConcurrency/Scheduler/Inline.hpp>

#include <utility>  // std::move

namespace DummyConcurrency::Future {

    // Consume Future, ignore value

    template <typename T>
    void Detach(Future<T> future) {
        std::move(future).Consume([](auto...) {}, sched::Inline());
    }

    // Chaining

    namespace pipe {

        struct [[nodiscard]] Detach {
            template <typename T>
            void Pipe(Future<T> f) {
                future::Detach(std::move(f));
            }
        };

    }  // namespace pipe

    inline auto Detach() {
        return pipe::Detach{};
    }

}  // namespace DummyConcurrency::Future
