#pragma once

#include "Awaiter.hpp"

#include <DummyConcurrency/Fiber/Core/Awaiter.hpp>

namespace NDummyConcurrency::NSynchronization {

    class IFiberAwaiter : public NFiber::IAwaiter, public IAwaiter {
    public:
        virtual void Wake() noexcept override final;

        void AfterSuspend();
        void EnableSymmetricContextSwitch();

    private:
        bool symmetric_switch_ = false;
    };

}  // namespace NDummyConcurrency::NSynchronization
