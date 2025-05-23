#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include "DummyConcurrency/Synchronization/Awaiters/Awaiter.hpp"

namespace NDummyConcurrency::NSynchronization::NFiberAware {

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
        static inline IAwaiter* const kUnlocked = reinterpret_cast<IAwaiter*>(0);
        static inline IAwaiter* const kLocked   = reinterpret_cast<IAwaiter*>(1);

        NImplementationLayer::Atomic<IAwaiter*> head_ = kUnlocked;
        IAwaiter*                              tail_ = nullptr;
    };

}  // namespace NDummyConcurrency::NSynchronization::FiberAware
