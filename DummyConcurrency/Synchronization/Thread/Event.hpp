#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Synchronization::Thread {

    class Event {
    public:
        // One-shot
        void Fire();
        void Wait();
        bool IsFired() const;

    private:
        ImplementationLayer::Atomic<uint32_t> state_ = 0;
    };

}  // namespace DummyConcurrency::Synchronization::Thread
