#pragma once

#include "IntrusiveQueue.hpp"

#include "DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp"
#include "DummyConcurrency/Scheduler/Interface/Scheduler.hpp"
#include "DummyConcurrency/Scheduler/Interface/Task.hpp"

#include <vector>

// Fixed-size pool of worker threads
namespace DummyConcurrency::Scheduler {

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
        static void Worker(ThreadPool* current_thread_pool);

        IntrusiveUnboundedBlockingQueue<ITask> task_queue_;
        std::vector<ImplementationLayer::Thread>       workers_;
        uint32_t                                       workers_amount_;
        bool                                           stopped_ = false;
    };

}  // namespace DummyConcurrency::Scheduler