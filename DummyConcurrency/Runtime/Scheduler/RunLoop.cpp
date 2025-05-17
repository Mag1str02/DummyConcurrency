#include "RunLoop.hpp"

#include <DummyConcurrency/Runtime/Invoker/Inline.hpp>

namespace NDummyConcurrency::NRuntime {

    namespace {
        class TaskProvider {
        public:
        private:
        };
    }  // namespace

    RunLoop::RunLoop() : invoker_(&InlineInvoker()) {}

    void RunLoop::Submit(ITask* task) {
        DC_PROFILE_SCOPE();
        queue_.PushBack(task);
    }

    void RunLoop::SetInvoker(IInvoker* invoker) {
        invoker_ = invoker;
    }
    size_t RunLoop::RunAtMost(size_t limit) {
        DC_PROFILE_SCOPE();
        current_run_limit_  = limit;
        current_task_count_ = 0;
        invoker_->Invoke(this);
        return current_task_count_;
    }

    size_t RunLoop::Run() {
        DC_PROFILE_SCOPE();
        current_run_limit_  = 0;
        current_task_count_ = 0;
        invoker_->Invoke(this);
        return current_task_count_;
    }

    bool RunLoop::RunNext() {
        DC_PROFILE_SCOPE();
        return RunAtMost(1) == 1;
    }

    bool RunLoop::IsEmpty() const {
        return queue_.IsEmpty();
    }
    bool RunLoop::NonEmpty() const {
        return !IsEmpty();
    }

    ITask* RunLoop::GetNextTask() noexcept {
        DC_PROFILE_SCOPE();
        if (!queue_.IsEmpty() && (current_run_limit_ == 0 || current_task_count_ < current_run_limit_)) {
            ++current_task_count_;
            return queue_.PopFront();
        }
        return nullptr;
    }

}  // namespace NDummyConcurrency::NRuntime
