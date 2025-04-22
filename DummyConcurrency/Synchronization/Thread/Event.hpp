#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace NDummyConcurrency::NSynchronization::NThread {

    class Event {
    public:
        // One-shot
        void Fire();
        void Wait();
        bool IsFired() const;

    private:
        NImplementationLayer::Atomic<uint32_t> state_ = 0;
    };

}  // namespace NDummyConcurrency::NSynchronization::Thread
