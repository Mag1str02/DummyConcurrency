#pragma once

#include "FiberAwaiter.hpp"
#include "ThreadAwaiter.hpp"

#include <DummyConcurrency/Fiber/Core/Fiber.hpp>
#include <DummyConcurrency/Fiber/Scheduling/Suspend.hpp>

namespace DummyConcurrency::Synchronization {

    template <typename F>
    void DoAwait(F&& function) {
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

        if (Fiber::Fiber::Self()) {
            FiberAwaiter awaiter(std::move(function));
            Fiber::Suspend(awaiter);
        } else {
            ThreadAwaiter awaiter;
            if (function(&awaiter)) {
                awaiter.Wait();
            }
        }
    }

}  // namespace DummyConcurrency::Synchronization
