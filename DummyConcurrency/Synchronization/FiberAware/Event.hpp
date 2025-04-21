#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Synchronization/Awaiters/Awaiter.hpp>

namespace DummyConcurrency::Synchronization::FiberAware {

    // One-shot, lock-free, zero-alloc
    class Event {
    public:
        void Wait();
        void Fire();

    private:
        static inline IAwaiter* const kFired    = reinterpret_cast<IAwaiter*>(0);
        static inline IAwaiter* const kNotFired = reinterpret_cast<IAwaiter*>(1);

        ImplementationLayer::Atomic<IAwaiter*> queue_head_ = kNotFired;
    };
}  // namespace DummyConcurrency::Synchronization::FiberAware
