#pragma once

#include <DummyConcurrency/Runtime/Scheduler/Task.hpp>

namespace NDummyConcurrency::NRuntime {

    class ITaskProvider {
    public:
        virtual ~ITaskProvider()              = default;
        virtual ITask* GetNextTask() noexcept = 0;
    };

    class IInvoker {
    public:
        virtual ~IInvoker() = default;
        // Return false if task provider has no task, true otherwise
        virtual void Invoke(ITaskProvider* provider) noexcept = 0;
    };

}  // namespace NDummyConcurrency::NRuntime
