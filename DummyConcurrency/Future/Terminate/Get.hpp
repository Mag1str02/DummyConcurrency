#pragma once

#include <DummyConcurrency/Future/Syntax/Pipe.hpp>

#include <DummyConcurrency/Synchronization/Event.hpp>

#include <DummyConcurrency/Util/ManualLifetime.hpp>

#include <DummyConcurrency/Scheduler/Inline.hpp>

#include <cstdlib>  // std::abort
#include <utility>  // std::move

namespace DummyConcurrency::Future {

    /*
     * Unwrap Future synchronously (blocking current thread)
     *
     * Usage:
     *
     * future::Get(future::Submit(pool, [] { return 7; }));
     *
     */

    template <typename T>
    T Get(Future<T> future) {
        thread::Event     event;
        ManualLifetime<T> value_container;
        std::move(future).Consume(
            [&event, &value_container](T value) {
                value_container.Construct(std::move(value));
                event.Fire();
            },
            sched::Inline());
        event.Wait();
        return value_container.Move();
    }

    // Chaining

    namespace pipe {

        struct [[nodiscard]] Get {
            template <typename T>
            T Pipe(Future<T> f) {
                return future::Get(std::move(f));
            }
        };

    }  // namespace pipe

    /*
     * Usage:
     *
     * auto v = future::Submit(pool, { return 7; }) | future::Get();
     */

    inline auto Get() {
        return pipe::Get{};
    }

}  // namespace DummyConcurrency::Future
