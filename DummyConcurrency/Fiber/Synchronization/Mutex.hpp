#pragma once

#include "DummyConcurrency/Fiber/Core/Awaiter.hpp"

#include <DummyConcurrency/ImplementationLayer.hpp>

namespace DummyConcurrency::Fiber {

    class Mutex {
    public:
        void Lock();
        bool TryLock();
        void Unlock();

        // Lockable
        void lock();      // NOLINT
        bool try_lock();  // NOLINT
        void unlock();    // NOLINT

    private:
        class Awaiter : public IAwaiter {
        public:
            explicit Awaiter(Mutex* mutex);
            virtual void OnSuspend() noexcept override;

        public:
            Awaiter* Next;

        private:
            Mutex* mutex_;
        };

        static inline Awaiter* const kUnlocked = reinterpret_cast<Awaiter*>(0);
        static inline Awaiter* const kLocked   = reinterpret_cast<Awaiter*>(1);

        ImplementationLayer::Atomic<Awaiter*> head_ = kUnlocked;
        Awaiter*                              tail_ = nullptr;

        FiberHandle unlocker_;
    };

}  // namespace DummyConcurrency::Fiber
