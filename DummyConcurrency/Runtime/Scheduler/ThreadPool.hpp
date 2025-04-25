#pragma once

#include <DummyConcurrency/DataStructures/IntrusiveUnboundedBlockingQueue.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Runtime/Invoker/Invoker.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Task.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <vector>

// Fixed-size pool of worker threads
namespace NDummyConcurrency::NRuntime {

    class ThreadPool : public IScheduler, public NonCopyable, public NonMovable, private ITaskProvider {
    public:
        explicit ThreadPool(size_t threads);
        ~ThreadPool();

        virtual void       Submit(ITask* task) override;
        static ThreadPool* Current();

        void Start();
        void Stop();
        void SetInvoker(IInvoker* invoker);

    private:
        using Queue = NDataStructures::IntrusiveUnboundedBlockingQueue<ITask>;
        static void Worker(ThreadPool* current_thread_pool);

        virtual ITask* GetNextTask() noexcept override;

    private:
        IInvoker*                                 invoker_;
        Queue                                     task_queue_;
        std::vector<NImplementationLayer::Thread> workers_;
        uint32_t                                  workers_amount_;
        bool                                      stopped_ = false;
    };

}  // namespace NDummyConcurrency::NRuntime