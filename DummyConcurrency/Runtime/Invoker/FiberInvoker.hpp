#pragma once

#include "Invoker.hpp"

#include <DummyConcurrency/Fiber/Stack/StackProvider.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>

namespace NDummyConcurrency::NRuntime {

    class FiberInvoker : public IInvoker {
    public:
        explicit FiberInvoker(NFiber::IStackProvider* stack_provider, IScheduler* scheduler);

        virtual void Invoke(ITaskProvider* provider) noexcept override;

    private:
        NFiber::IStackProvider* stack_provider_;
        IScheduler*             scheduler_;
    };

}  // namespace NDummyConcurrency::NRuntime
