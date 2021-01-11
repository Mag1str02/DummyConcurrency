#pragma once

#include "DummyConcurrency/Utils/IntrusiveForwardList.hpp"

#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/mutex.hpp>

namespace DummyConcurrency {

    // Unbounded blocking multi-producers/multi-consumers (MPMC) queue
    template <typename T> class IntrusiveUnboundedBlockingQueue {
    public:
        void Push(T* value) {
            std::lock_guard guard(mutex_);
            queue_.PushBack(value);
            pop_wait_.notify_one();
        }

        T* Pop() {
            std::unique_lock lock(mutex_);
            while (!closed_ && queue_.IsEmpty()) {
                pop_wait_.wait(lock);
            }
            if (!queue_.IsEmpty()) {
                return queue_.PopFront();
            }
            return nullptr;
        }

        void Close() {
            std::lock_guard guard(mutex_);
            closed_ = true;
            pop_wait_.notify_all();
        }

    private:
        IntrusiveForwardList<T>            queue_;
        twist::ed::std::mutex              mutex_;
        twist::ed::std::condition_variable pop_wait_;

        bool closed_ = false;
    };

}  // namespace DummyConcurrency
