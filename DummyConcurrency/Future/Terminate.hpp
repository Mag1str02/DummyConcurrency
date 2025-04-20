#pragma once

#include <DummyConcurrency/Future/Syntax/Pipe.hpp>
#include <DummyConcurrency/Scheduler/Inline.hpp>
#include <DummyConcurrency/Synchronization/Event.hpp>
#include <DummyConcurrency/Utils/ManualLifetime.hpp>

namespace DummyConcurrency::Future {

    template <typename T>
    T Get(Future<T> future) {
        Event             event;
        ManualLifetime<T> value_container;
        std::move(future).Consume(
            [&event, &value_container](T value) {
                value_container.Construct(std::move(value));
                event.Fire();
            },
            Scheduler::Inline());
        event.Wait();
        return value_container.Move();
    }

    template <typename T>
    void Detach(Future<T> future) {
        std::move(future).Consume([](auto...) {}, Scheduler::Inline());
    }

    namespace Combinators {

        struct [[nodiscard]] Get {
            template <typename T>
            T Pipe(Future<T> future) {
                return ::DummyConcurrency::Future::Get(std::move(future));
            }
        };

        struct [[nodiscard]] Detach {
            template <typename T>
            void Pipe(Future<T> future) {
                ::DummyConcurrency::Future::Detach(std::move(future));
            }
        };

    }  // namespace Combinators

    inline auto Get() {
        return Combinators::Get{};
    }
    inline auto Detach() {
        return Combinators::Detach{};
    }

}  // namespace DummyConcurrency::Future
