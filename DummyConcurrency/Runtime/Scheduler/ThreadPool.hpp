#pragma once

#include <DummyConcurrency/DataStructures/IntrusiveUnboundedBlockingQueue.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Task.hpp>

#include <vector>

// Fixed-size pool of worker threads
namespace NDummyConcurrency::NRuntime {

    class ThreadPool : public IScheduler {
    public:
        explicit ThreadPool(size_t threads);
        ~ThreadPool();

        // Non-copyable
        ThreadPool(const ThreadPool&)            = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        // Non-movable
        ThreadPool(ThreadPool&&)            = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        void Start();

        virtual void Submit(ITask* task) override;

        static ThreadPool* Current();

        void Stop();

    private:
        using Queue = NDataStructures::IntrusiveUnboundedBlockingQueue<ITask>;
        static void Worker(ThreadPool* current_thread_pool);

        Queue                                    task_queue_;
        std::vector<NImplementationLayer::Thread> workers_;
        uint32_t                                 workers_amount_;
        bool                                     stopped_ = false;
    };

}  // namespace NDummyConcurrency::NRuntime