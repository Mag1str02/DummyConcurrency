#include "RunLoop.hpp"

namespace DummyConcurrency::Scheduler {

    void RunLoop::Submit(ITask* task) {
        queue_.PushBack(task);
    }

    // Run tasks

    size_t RunLoop::RunAtMost(size_t limit) {
        size_t cnt = 0;
        while (!queue_.IsEmpty() && cnt < limit) {
            ITask* task = queue_.PopFront();
            task->Run();
            ++cnt;
        }
        return cnt;
    }

    size_t RunLoop::Run() {
        size_t cnt = 0;
        while (!queue_.IsEmpty()) {
            ITask* task = queue_.PopFront();
            task->Run();
            ++cnt;
        }
        return cnt;
    }

}  // namespace DummyConcurrency::Scheduler
