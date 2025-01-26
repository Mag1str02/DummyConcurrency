#include "ThreadPool.hpp"

#include <cassert>

namespace DummyConcurrency::Scheduler {

    STATIC_THREAD_LOCAL_PTR(ThreadPool, gCurrentThreadPool);

    ThreadPool::ThreadPool(size_t thread) : workers_amount_(thread) {}

    void ThreadPool::Start() {
        for (uint32_t i = 0; i < workers_amount_; ++i) {
            workers_.emplace_back(Worker, this);
        }
    }

    ThreadPool::~ThreadPool() {
        assert(stopped_);
    }

    void ThreadPool::Submit(IntrusiveTask* task) {
        assert(!stopped_);
        task_queue_.Push(std::move(task));
    }

    ThreadPool* ThreadPool::Current() {
        return gCurrentThreadPool;
    }

    void ThreadPool::Stop() {
        stopped_ = true;
        task_queue_.Close();
        for (auto& thread : workers_) {
            thread.join();
        }
    }

    void ThreadPool::Worker(ThreadPool* current_thread_pool) {
        gCurrentThreadPool = current_thread_pool;

        do {
            auto* current_task = Current()->task_queue_.Pop();
            if (current_task != nullptr) {
                current_task->Run();
            } else {
                break;
            }
        } while (true);
    }

}  // namespace DummyConcurrency::Scheduler
