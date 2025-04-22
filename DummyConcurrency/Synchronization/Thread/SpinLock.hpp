#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace NDummyConcurrency::NSynchronization::NThread {

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
        NImplementationLayer::Atomic<uint32_t> state_ = 0;
    };

}  // namespace NDummyConcurrency::NSynchronization::Thread
