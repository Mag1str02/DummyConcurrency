#pragma once

#include "DummyConcurrency/Fiber/Core/Awaiter.hpp"

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

namespace DummyConcurrency::Fiber {

    // One-shot, lock-free, zero-alloc
    class Event {
    public:
        void Wait();
        void Fire();

    private:
        class Awaiter : public IAwaiter {
        public:
            explicit Awaiter(Event* event);
            virtual void OnSuspend() noexcept override;

        public:
            Awaiter* Next;

        private:
            Event* event_;
        };

        static inline Awaiter* const kFired    = reinterpret_cast<Awaiter*>(0);
        static inline Awaiter* const kNotFired = reinterpret_cast<Awaiter*>(1);

        twist::ed::std::atomic<Awaiter*> queue_head_ = kNotFired;
    };
}  // namespace DummyConcurrency::Fiber
