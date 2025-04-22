#pragma once

#include "Awaiter.hpp"

#include <DummyConcurrency/Synchronization/Thread/Event.hpp>

namespace NDummyConcurrency::NSynchronization {

    class ThreadAwaiter : public IAwaiter {
    public:
        virtual void Wake() noexcept override final { event_.Fire(); }
        void         Wait() noexcept { event_.Wait(); };

    private:
        NThread::Event event_;
    };

}  // namespace NDummyConcurrency::Synchronization
