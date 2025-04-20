#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency {

    class Event {
    public:
        // One-shot
        void Fire();
        void Wait();

    private:
        ImplementationLayer::Atomic<uint32_t> state_ = 0;
    };

}  // namespace DummyConcurrency
