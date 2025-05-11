#include "Event.hpp"

namespace NDummyConcurrency::NSynchronization::NThread {

    void Event::Fire() {
        auto wake_key = NImplementationLayer::Futex::PrepareWake(state_);
        state_.store(1);
        NImplementationLayer::Futex::WakeAll(wake_key);
    }

    void Event::Wait() {
        while (state_.load() == 0) {
            NImplementationLayer::Futex::Wait(state_, 0);
        }
    }

    bool Event::IsFired() const {
        return state_.load() != 0;
    }

}  // namespace NDummyConcurrency::NSynchronization::Thread
