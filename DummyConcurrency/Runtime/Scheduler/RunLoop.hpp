#pragma once

#include "DummyConcurrency/Runtime/Invoker/Invoker.hpp"
#include "DummyConcurrency/Runtime/Scheduler/Scheduler.hpp"
#include "DummyConcurrency/Runtime/Scheduler/Task.hpp"
#include "DummyConcurrency/Utils/Traits.hpp"

namespace NDummyConcurrency::NRuntime {

    // Single-threaded task queue, non thread safe
    class RunLoop : public IScheduler, public NonCopyable, public NonMovable, private ITaskProvider {
    public:
        RunLoop();

        virtual void Submit(ITask* task) override;

        void   SetInvoker(IInvoker* invoker);
        size_t RunAtMost(size_t limit);
        size_t Run();
        bool   RunNext();

        bool IsEmpty() const;
        bool NonEmpty() const;

    private:
        virtual ITask* GetNextTask() noexcept override;

    private:
        IntrusiveForwardList<ITask> queue_;
        IInvoker*                   invoker_;
        size_t                      current_run_limit_  = 0;
        size_t                      current_task_count_ = 0;
    };

}  // namespace NDummyConcurrency::NRuntime
