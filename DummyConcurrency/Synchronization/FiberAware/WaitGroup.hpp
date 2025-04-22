#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Synchronization/Awaiters/Awaiter.hpp>
#include <DummyConcurrency/Synchronization/Thread/SpinLock.hpp>
#include <DummyConcurrency/Utils/IntrusiveForwardList.hpp>
#include <DummyConcurrency/Utils/StampedPtr.hpp>

namespace NDummyConcurrency::NSynchronization::NFiberAware {

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
        NSynchronization::NThread::SpinLock lock_;
        uint32_t                          counter_ = 0;
        IntrusiveForwardList<IAwaiter>    wait_queue_;
    };

    // lock-free, zero-alloc
    // assumes that pointers use only first 48 bits, and counter is limited to 65535
    class BadPointerWaitGroup {
    public:
        void Add(size_t count);
        void Done();
        void Wait();

    private:
        AtomicStampedPtr<IAwaiter> state_;
    };

}  // namespace NDummyConcurrency::NSynchronization::FiberAware
