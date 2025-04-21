#include "Mutex.hpp"

#include "DummyConcurrency/Fiber/Scheduling/Suspend.hpp"

namespace DummyConcurrency::Synchronization::FiberAware {

    void Mutex::Lock() {
        Awaiter* expected = kUnlocked;
        if (head_.compare_exchange_strong(expected, kLocked)) {
            return;
        }
        Awaiter awaiter(this);
        Suspend(awaiter);
        if (unlocker_.IsValid()) {
            unlocker_.Schedule();
        }
    }

    bool Mutex::TryLock() {
        Awaiter* expected = kUnlocked;
        return head_.compare_exchange_strong(expected, kLocked);
    }

    void Mutex::Unlock() {
        if (tail_ == kUnlocked) {
            Awaiter* expected_head = head_.load();
            Awaiter* new_head;
            do {
                new_head = kLocked;
                if (expected_head == kLocked) {
                    new_head = kUnlocked;
                }
            } while (!head_.compare_exchange_weak(expected_head, new_head));
            if (new_head == kUnlocked) {
                return;
            }
            while (expected_head != kLocked) {
                Awaiter* next       = expected_head->Next;
                expected_head->Next = tail_;
                tail_               = expected_head;
                expected_head       = next;
            }
        }
        if (tail_ != kUnlocked) {
            Awaiter* curr = tail_;
            tail_         = tail_->Next;
            curr->Switch(unlocker_);
        }
    }

    // Lockable
    void Mutex::lock() {  // NOLINT
        Lock();
    }

    bool Mutex::try_lock() {  // NOLINT
        return TryLock();
    }

    void Mutex::unlock() {  // NOLINT
        Unlock();
    }
    Mutex::Awaiter::Awaiter(Mutex* mutex) : mutex_(mutex) {}
    void Mutex::Awaiter::OnSuspend() noexcept {
        Awaiter* expected = mutex_->head_.load();
        Awaiter* new_state;
        do {
            new_state = mutex_->kLocked;
            Next      = kUnlocked;
            if (expected != kUnlocked) {
                new_state = this;
                Next      = expected;
            }
        } while (!mutex_->head_.compare_exchange_weak(expected, new_state));
        if (new_state != this) {
            Schedule();
        }
    }

}  // namespace DummyConcurrency::Fiber
