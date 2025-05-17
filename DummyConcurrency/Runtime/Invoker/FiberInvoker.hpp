#pragma once

#include "Invoker.hpp"

#include <DummyConcurrency/Fiber/Core/Hint.hpp>
#include <DummyConcurrency/Fiber/Stack/StackProvider.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>

namespace NDummyConcurrency::NRuntime {

    class IFiberHintProvider {
    public:
        virtual NFiber::Hint GetHint()                = 0;
        virtual void         ReturnHint(NFiber::Hint) = 0;
    };

    class FiberInvoker : public IInvoker {
    public:
        explicit FiberInvoker(IFiberHintProvider* hint_provider, IScheduler* scheduler);

        virtual void Invoke(ITaskProvider* provider) noexcept override;

    private:
        void Iteration(ITaskProvider* task_provider);

    private:
        IFiberHintProvider* hint_provider_;
        IScheduler*         scheduler_;
    };

}  // namespace NDummyConcurrency::NRuntime
