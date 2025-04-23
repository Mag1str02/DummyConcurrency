#pragma once

#include "FiberAwaiter.hpp"
#include "ThreadAwaiter.hpp"

#include <DummyConcurrency/Fiber/Core/Fiber.hpp>
#include <DummyConcurrency/Fiber/Scheduling/Suspend.hpp>

namespace NDummyConcurrency::NSynchronization {

    template <typename F>
    void DoAwait(F&& function, bool symetric_context_switch = false) {
        class FiberAwaiter : public IFiberAwaiter {
        public:
            explicit FiberAwaiter(F&& function) : function_(std::move(function)) {}
            virtual void OnSuspend() noexcept override {
                if (!function_(this)) {
                    Schedule();
                }
            }

        private:
            F function_;
        };

        if (NFiber::Fiber::Self()) {
            FiberAwaiter awaiter(std::move(function));
            if (symetric_context_switch) {
                awaiter.EnableSymmetricContextSwitch();
            }
            NFiber::Suspend(awaiter);
            awaiter.AfterSuspend();
        } else {
            ThreadAwaiter awaiter;
            if (function(&awaiter)) {
                awaiter.Wait();
            }
        }
    }

}  // namespace NDummyConcurrency::NSynchronization
