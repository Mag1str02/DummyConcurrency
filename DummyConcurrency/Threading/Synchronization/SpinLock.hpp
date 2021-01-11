#pragma once

#include <twist/ed/std/atomic.hpp>

namespace DummyConcurrency::Threading {

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
        twist::ed::std::atomic<uint32_t> state_ = 0;
    };

}  // namespace DummyConcurrency::Threading
