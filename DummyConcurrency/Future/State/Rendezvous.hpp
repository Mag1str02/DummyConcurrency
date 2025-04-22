#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Future {

    class RendezvousStateMachine {
    public:
        bool Produce() { return state_.fetch_or(Produced) == Consumed; }
        bool Consume() { return state_.fetch_or(Consumed) == Produced; }
        bool IsReady() const { return (state_.load() | Produced) != 0; }

    private:
        enum State {
            Init       = 0,
            Produced   = 1,
            Consumed   = 2,
            Rendezvous = Consumed | Produced,
        };

        ImplementationLayer::Atomic<uint32_t> state_{Init};
    };

}  // namespace DummyConcurrency::Future
