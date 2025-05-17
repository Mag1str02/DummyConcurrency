#include "ThreadPool.hpp"

#include <DummyConcurrency/Fiber/Stack/InlineStackPool.hpp>
#include <DummyConcurrency/Fiber/Stack/StackPool.hpp>
#include <DummyConcurrency/Runtime/Invoker/FiberInvoker.hpp>
#include <DummyConcurrency/Runtime/Invoker/Inline.hpp>

namespace NDummyConcurrency::NRuntime {

    STATIC_THREAD_LOCAL_PTR(ThreadPool, gCurrentThreadPool);

    ThreadPool::ThreadPool(IThreadFactory* factory, size_t thread) : thread_factory_(factory), invoker_(&InlineInvoker()), workers_amount_(thread) {
        DC_ASSERT(workers_amount_ > 0, "ThreadPool size cannot be zero");
    }

    ThreadPool::~ThreadPool() {
        DC_ASSERT(stopped_, "Thread pool was destructed before calling stop");
    }

    void ThreadPool::Submit(ITask* task) {
        DC_PROFILE_SCOPE();
        DC_ASSERT(!stopped_, "Thread pool is stopped");
        task_queue_.Push(std::move(task));
    }
    ThreadPool* ThreadPool::Current() {
        return gCurrentThreadPool;
    }

    void ThreadPool::Start() {
        DC_PROFILE_SCOPE();
        for (uint32_t i = 0; i < workers_amount_; ++i) {
            workers_.emplace_back(thread_factory_->LaunchThread([this]() { Worker(this); }));
        }
    }
    void ThreadPool::Stop() {
        DC_PROFILE_SCOPE();
        DC_ASSERT(!workers_.empty(), "ThreadPool stopped before it was started");
        stopped_ = true;
        task_queue_.Close();
        for (auto& thread : workers_) {
            thread.join();
        }
    }
    void ThreadPool::SetInvoker(IInvoker* invoker) {
        DC_ASSERT(workers_.empty(), "Cannot set invoker after thread pool start");
        invoker_ = invoker;
    }

    void ThreadPool::Worker(ThreadPool* current_thread_pool) {
        gCurrentThreadPool = current_thread_pool;

        current_thread_pool->invoker_->Invoke(current_thread_pool);
    }
    ITask* ThreadPool::GetNextTask() noexcept {
        DC_PROFILE_SCOPE();
        return task_queue_.Pop();
    }

}  // namespace NDummyConcurrency::NRuntime
