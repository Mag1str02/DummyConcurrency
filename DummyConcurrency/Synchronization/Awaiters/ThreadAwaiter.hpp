#pragma once

#include "Awaiter.hpp"

#include <DummyConcurrency/Synchronization/Thread/Event.hpp>

namespace DummyConcurrency::Synchronization {

    class ThreadAwaiter : public IAwaiter {
    public:
        virtual void Wake() noexcept override final { event_.Fire(); }
        virtual void Wait() noexcept override final { event_.Wait(); };

    private:
        Thread::Event event_;
    };

}  // namespace DummyConcurrency::Synchronization
