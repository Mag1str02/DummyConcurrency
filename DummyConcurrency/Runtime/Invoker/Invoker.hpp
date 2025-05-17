#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Task.hpp>

namespace NDummyConcurrency::NRuntime {

    class ITaskProvider {
    public:
        virtual ~ITaskProvider()              = default;
        virtual ITask* GetNextTask() noexcept = 0;
    };

    class IInvoker {
    public:
        virtual ~IInvoker()                                   = default;
        virtual void Invoke(ITaskProvider* provider) noexcept = 0;
    };

}  // namespace NDummyConcurrency::NRuntime
