#pragma once

#include <DummyConcurrency/DataStructures/IntrusiveUnboundedBlockingQueue.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Runtime/Invoker/Invoker.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Task.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

// ThreadPool in which other threads can become workers

namespace NDummyConcurrency::NRuntime {

    class ExternalThreadPool : public IScheduler, public NonCopyable, public NonMovable, private ITaskProvider {
    public:
        explicit ExternalThreadPool() = default;
        ~ExternalThreadPool();

        virtual void Submit(ITask* task) override;

        void Stop();
        void BecomeWorker();

    private:
        using Queue = NDataStructures::IntrusiveUnboundedBlockingQueue<ITask>;

        virtual ITask* GetNextTask() noexcept override;

    private:
        Queue                              task_queue_;
        NImplementationLayer::Atomic<bool> stopped_ = false;
    };

}  // namespace NDummyConcurrency::NRuntime