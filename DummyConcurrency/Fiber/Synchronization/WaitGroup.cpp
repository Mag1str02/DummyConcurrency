#include "WaitGroup.hpp"

#include "DummyConcurrency/Fiber/Scheduling/Suspend.hpp"

#include <mutex>

namespace DummyConcurrency::Fiber {

    namespace {
        static constexpr uint64_t kPointerBits = 48;
        static constexpr uint64_t kPointerMask = (1ull << kPointerBits) - 1;
        static constexpr uint64_t kCounterMask = ~kPointerMask;
        static constexpr uint64_t GetCounter(uint64_t val) {
            return val >> kPointerBits;
        }
        static constexpr uint64_t AddCounter(uint64_t val, size_t delta) {
            uint64_t counter = val >> kPointerBits;
            counter += delta;
            return (counter << kPointerBits) + (val & kPointerMask);
        }

    }  // namespace

    void SpinlockWaitGroup::Add(size_t count) {
        std::lock_guard guard(lock_);
        counter_ += count;
    }

    void SpinlockWaitGroup::Done() {
        std::unique_lock guard(lock_);
        if (--counter_ == 0) {
            IntrusiveForwardList<Awaiter> buffer;
            while (wait_queue_.NonEmpty()) {
                buffer.PushBack(wait_queue_.PopFront());
            }
            guard.unlock();
            while (buffer.NonEmpty()) {
                Awaiter* awaiter = buffer.PopFront();
                awaiter->Schedule();
            }
        }
    }

    void SpinlockWaitGroup::Wait() {
        lock_.Lock();
        if (counter_ == 0) {
            lock_.Unlock();
            return;
        }
        Awaiter awaiter(this);
        Suspend(awaiter);
    }

    SpinlockWaitGroup::Awaiter::Awaiter(SpinlockWaitGroup* wg) : wg_(wg) {}
    void SpinlockWaitGroup::Awaiter::OnSuspend() noexcept {
        wg_->wait_queue_.PushBack(this);
        wg_->lock_.Unlock();
    }

    // lock-free, zero-alloc
    // assumes that pointers use only first 48 bits, and counter is limited to 65535
    void BadPointerWaitGroup::Add(size_t count) {
        state_.fetch_add((uint64_t)count << kPointerBits);
    }
    void BadPointerWaitGroup::Done() {
        uint64_t state = state_.load();
        uint64_t new_state;
        do {
            new_state = AddCounter(state, -1);
            if (GetCounter(state) == 1) {
                new_state = 0;
            }
        } while (!state_.compare_exchange_strong(state, new_state));
        if (GetCounter(state) == 1) {
            Awaiter* head = GetPointer(state);
            while (head != nullptr) {
                Awaiter* next = head->Next;
                head->Schedule();
                head = next;
            }
        }
    }
    void BadPointerWaitGroup::Wait() {
        uint64_t state = state_.load();
        if (GetCounter(state) == 0) {
            return;
        }
        Awaiter awaiter(this);
        Suspend(awaiter);
    }

    BadPointerWaitGroup::Awaiter::Awaiter(BadPointerWaitGroup* wg) : wg_(wg) {}
    void BadPointerWaitGroup::Awaiter::OnSuspend() noexcept {
        uint64_t state = wg_->state_.load();
        do {
            if (GetCounter(state) == 0) {
                Schedule();
                return;
            }
            Next = GetPointer(state);
        } while (!wg_->state_.compare_exchange_strong(state, SetPointer(state, this)));
    }

    BadPointerWaitGroup::Awaiter* BadPointerWaitGroup::GetPointer(uint64_t val) {
        return reinterpret_cast<Awaiter*>(val & kPointerMask);
    }
    uint64_t BadPointerWaitGroup::SetPointer(uint64_t val, BadPointerWaitGroup::Awaiter* ptr) {
        uint64_t pointer = reinterpret_cast<uint64_t>(ptr);
        DC_ASSERT((pointer & kCounterMask) == 0, "Pointer is longer than 48 bits");
        return pointer + (val & kCounterMask);
    }

}  // namespace DummyConcurrency::Fiber