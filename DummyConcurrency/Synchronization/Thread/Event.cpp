#include "Event.hpp"

namespace DummyConcurrency::Synchronization::Thread {

    void Event::Fire() {
        auto wake_key = ImplementationLayer::Futex::PrepareWake(state_);
        state_.store(1);
        ImplementationLayer::Futex::WakeAll(wake_key);
    }

    void Event::Wait() {
        if (state_.load() == 0) {
            ImplementationLayer::Futex::Wait(state_, 0);
        }
    }

    bool Event::IsFired() const {
        return state_.load() != 0;
    }

}  // namespace DummyConcurrency::Synchronization::Thread
