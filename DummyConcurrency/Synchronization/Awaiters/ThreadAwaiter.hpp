#pragma once

#include "Awaiter.hpp"

#include <DummyConcurrency/Synchronization/Thread/Event.hpp>

namespace DummyConcurrency::Synchronization {

    class ThreadAwaiter : public IAwaiter {
    public:
        virtual void Wake() noexcept override final {
            if (has_wait_.load()) {
                event_.Fire();
            }
        }
        virtual void Wait() noexcept override final {
            has_wait_.store(true);
            event_.Wait();
        };

    private:
        Thread::Event                     event_;
        ImplementationLayer::Atomic<bool> has_wait_ = false;
    };

}  // namespace DummyConcurrency::Synchronization
