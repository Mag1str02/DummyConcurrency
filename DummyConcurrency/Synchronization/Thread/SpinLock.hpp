#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Synchronization::Thread {

    class SpinLock {
    public:
        void Lock();
        bool TryLock();
        void Unlock();

        // Lockable

        void lock();      // NOLINT
        bool try_lock();  // NOLINT
        void unlock();    // NOLINT

    private:
        ImplementationLayer::Atomic<uint32_t> state_ = 0;
    };

}  // namespace DummyConcurrency::Synchronization::Thread
