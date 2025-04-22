#include "Mutex.hpp"

#include "DummyConcurrency/Synchronization/Awaiters/DoAwait.hpp"

namespace NDummyConcurrency::NSynchronization::NFiberAware {

    void Mutex::Lock() {
        IAwaiter* expected = kUnlocked;
        if (head_.compare_exchange_strong(expected, kLocked)) {
            return;
        }
        DoAwait([this](IAwaiter* current_awaiter) {
            IAwaiter* expected = head_.load();
            IAwaiter* new_state;
            do {
                new_state             = kLocked;
                current_awaiter->Next = kUnlocked;
                if (expected != kUnlocked) {
                    new_state             = current_awaiter;
                    current_awaiter->Next = expected;
                }
            } while (!head_.compare_exchange_weak(expected, new_state));
            return new_state == current_awaiter;
        });
    }

    bool Mutex::TryLock() {
        IAwaiter* expected = kUnlocked;
        return head_.compare_exchange_strong(expected, kLocked);
    }

    void Mutex::Unlock() {
        if (tail_ == kUnlocked) {
            IAwaiter* expected_head = head_.load();
            IAwaiter* new_head;
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
                IAwaiter* next      = expected_head->Next;
                expected_head->Next = tail_;
                tail_               = expected_head;
                expected_head       = next;
            }
        }
        if (tail_ != kUnlocked) {
            IAwaiter* curr = tail_;
            tail_          = tail_->Next;
            curr->Wake();
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

}  // namespace NDummyConcurrency::NSynchronization::FiberAware
