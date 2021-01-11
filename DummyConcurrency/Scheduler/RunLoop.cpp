#include "RunLoop.hpp"

namespace DummyConcurrency::Scheduler {

    void RunLoop::Submit(IntrusiveTask* task) {
        queue_.PushBack(task);
    }

    // Run tasks

    size_t RunLoop::RunAtMost(size_t limit) {
        size_t cnt = 0;
        while (!queue_.IsEmpty() && cnt < limit) {
            IntrusiveTask* task = queue_.PopFront();
            task->Run();
            ++cnt;
        }
        return cnt;
    }

    size_t RunLoop::Run() {
        size_t cnt = 0;
        while (!queue_.IsEmpty()) {
            IntrusiveTask* task = queue_.PopFront();
            task->Run();
            ++cnt;
        }
        return cnt;
    }

}  // namespace DummyConcurrency::Scheduler
