#include "ExternalThreadPool.hpp"

#include <DummyConcurrency/Runtime/Invoker/Inline.hpp>

namespace NDummyConcurrency::NRuntime {

    ExternalThreadPool::~ExternalThreadPool() {
        DC_ASSERT(stopped_, "Thread pool was destructed before calling stop");
    }

    void ExternalThreadPool::Submit(ITask* task) {
        DC_ASSERT(!stopped_, "ExternalThreadPool is stopped");
        task_queue_.Push(std::move(task));
    }
    void ExternalThreadPool::Stop() {
        stopped_ = true;
        task_queue_.Close();
    }

    void ExternalThreadPool::BecomeWorker() {
        InlineInvoker().Invoke(this);
    }
    ITask* ExternalThreadPool::GetNextTask() noexcept {
        return task_queue_.Pop();
    }

}  // namespace NDummyConcurrency::NRuntime
