#include "WaitGroup.hpp"

#include <DummyConcurrency/Synchronization/Awaiters/DoAwait.hpp>

#include <mutex>

namespace DummyConcurrency::Synchronization::FiberAware {

    void SpinlockWaitGroup::Add(size_t count) {
        std::lock_guard guard(lock_);
        counter_ += count;
    }
    void SpinlockWaitGroup::Done() {
        std::unique_lock guard(lock_);
        if (--counter_ == 0) {
            IntrusiveForwardList<IAwaiter> buffer;
            while (wait_queue_.NonEmpty()) {
                buffer.PushBack(wait_queue_.PopFront());
            }
            guard.unlock();
            while (buffer.NonEmpty()) {
                IAwaiter* awaiter = buffer.PopFront();
                awaiter->Wake();
            }
        }
    }
    void SpinlockWaitGroup::Wait() {
        lock_.Lock();
        if (counter_ == 0) {
            lock_.Unlock();
            return;
        }
        DoAwait([this](IAwaiter* current_awaiter) {
            wait_queue_.PushBack(current_awaiter);
            lock_.Unlock();
            return true;
        });
    }

    void BadPointerWaitGroup::Add(size_t count) {
        StampedPtr<IAwaiter> ptr = state_.Load();
        StampedPtr<IAwaiter> new_ptr;
        do {
            DC_ASSERT(uint64_t(ptr.Stamp) + count <= StampedPtr<IAwaiter>::kMaxStamp, "Invalid stamp");
            new_ptr.RawPtr = ptr.RawPtr;
            new_ptr.Stamp  = ptr.Stamp + count;
        } while (!state_.CompareExchangeWeak(ptr, new_ptr));
    }
    void BadPointerWaitGroup::Done() {
        StampedPtr<IAwaiter> state = state_.Load();
        StampedPtr<IAwaiter> new_state;
        do {
            DC_ASSERT(state.Stamp != 0, "Done called before corresponding add");
            if (state.Stamp == 1) {
                new_state.RawPtr = nullptr;
                new_state.Stamp  = 0;
            } else {
                new_state.RawPtr = state.RawPtr;
                new_state.Stamp  = state.Stamp - 1;
            }
        } while (!state_.CompareExchangeWeak(state, new_state));
        if (state.Stamp == 1) {
            IAwaiter* head = state.RawPtr;
            while (head != nullptr) {
                IAwaiter* next = head->Next;
                head->Wake();
                head = next;
            }
        }
    }
    void BadPointerWaitGroup::Wait() {
        auto state = state_.Load();
        if (state.Stamp == 0) {
            return;
        }
        DoAwait([this](IAwaiter* current_awaiter) {
            StampedPtr<IAwaiter> state = state_.Load();
            StampedPtr<IAwaiter> new_state;
            do {
                if (state.Stamp == 0) {
                    return false;
                }
                current_awaiter->Next = state.RawPtr;
                new_state.RawPtr      = current_awaiter;
                new_state.Stamp       = state.Stamp;
            } while (!state_.CompareExchangeWeak(state, new_state));
            return true;
        });
    }

}  // namespace DummyConcurrency::Synchronization::FiberAware