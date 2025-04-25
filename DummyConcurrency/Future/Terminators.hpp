#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>
#include <DummyConcurrency/Synchronization/FiberAware/Event.hpp>
#include <DummyConcurrency/Utils/ManualLifetime.hpp>

namespace NDummyConcurrency::NFuture {

    template <typename T>
    T Get(Future<T> future) {
        NSynchronization::NFiberAware::Event event;
        ManualLifetime<T>                    value_container;
        std::move(future).Consume(
            [&event, &value_container](T value) {
                value_container.Construct(std::move(value));
                event.Fire();
            },
            NRuntime::Inline());
        event.Wait();
        return value_container.Move();
    }

    template <typename T>
    void Detach(Future<T> future) {
        std::move(future).Consume([](auto...) {}, NRuntime::Inline());
    }

}  // namespace NDummyConcurrency::NFuture
