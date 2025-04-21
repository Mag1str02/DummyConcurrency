#include "SpinLock.hpp"

namespace DummyConcurrency::Synchronization::Thread {

    void SpinLock::Lock() {
        while (state_.exchange(1) == 1) {
            ImplementationLayer::SpinWait spin_wait;
            while (state_.load() == 1) {
                spin_wait();
            }
        }
    }

    bool SpinLock::TryLock() {
        return state_.exchange(1) == 0;
    }

    void SpinLock::Unlock() {
        state_.store(0);
    }

    // Lockable

    void SpinLock::lock() {  // NOLINT
        Lock();
    }

    bool SpinLock::try_lock() {  // NOLINT
        return TryLock();
    }

    void SpinLock::unlock() {  // NOLINT
        Unlock();
    }

}  // namespace DummyConcurrency::Synchronization::Thread