#pragma once

#include "DummyConcurrency/Fiber/Core/Awaiter.hpp"
#include "DummyConcurrency/Threading/Synchronization/SpinLock.hpp"
#include "DummyConcurrency/Utils/IntrusiveForwardList.hpp"

#include <twist/ed/std/atomic.hpp>

namespace DummyConcurrency::Fiber {

    class SpinlockWaitGroup;
    class BadPointerWaitGroup;

    using WaitGroup = BadPointerWaitGroup;

    // locking, zero-alloc
    class SpinlockWaitGroup {
    public:
        void Add(size_t count);

        void Done();

        void Wait();

    private:
        class Awaiter : public IAwaiter, public IntrusiveForwardListNode<Awaiter> {
        public:
            explicit Awaiter(SpinlockWaitGroup* wg);
            virtual void OnSuspend() noexcept override;

        private:
            SpinlockWaitGroup* wg_;
        };

        Threading::SpinLock           lock_;
        uint32_t                      counter_ = 0;
        IntrusiveForwardList<Awaiter> wait_queue_;
    };

    // lock-free, zero-alloc
    // assumes that pointers use only first 48 bits, and counter is limited to 65535
    class BadPointerWaitGroup {
    public:
        void Add(size_t count);
        void Done();
        void Wait();

    private:
        class Awaiter : public IAwaiter {
        public:
            explicit Awaiter(BadPointerWaitGroup* wg);
            virtual void OnSuspend() noexcept override;

        public:
            Awaiter* Next = nullptr;

        private:
            BadPointerWaitGroup* wg_;
        };

        static Awaiter* GetPointer(uint64_t val);
        static uint64_t SetPointer(uint64_t val, Awaiter* ptr);

        twist::ed::std::atomic<uint64_t> state_ = 0;
    };

}  // namespace DummyConcurrency::Fiber
