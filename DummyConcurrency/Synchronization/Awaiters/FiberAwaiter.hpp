#pragma once

#include "Awaiter.hpp"

#include <DummyConcurrency/Fiber/Core/Awaiter.hpp>

namespace DummyConcurrency::Synchronization {

    class IFiberAwaiter : public Fiber::IAwaiter, public IAwaiter {
    public:
        virtual void Wake() noexcept override final { Schedule(); }
        virtual void Wait() noexcept override final {};
    };

}  // namespace DummyConcurrency::Synchronization
