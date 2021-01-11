#pragma once

#include "DummyConcurrency/Scheduler/Interface/Scheduler.hpp"
#include "DummyConcurrency/Scheduler/Interface/Task.hpp"

namespace DummyConcurrency::Scheduler {

    // Single-threaded task queue

    class RunLoop : public IScheduler {
    public:
        RunLoop() = default;

        // Non-copyable
        RunLoop(const RunLoop&)            = delete;
        RunLoop& operator=(const RunLoop&) = delete;

        // Non-movable
        RunLoop(RunLoop&&)            = delete;
        RunLoop& operator=(RunLoop&&) = delete;

        // task::IScheduler
        void Submit(IntrusiveTask* task) override;

        // Run tasks

        size_t RunAtMost(size_t limit);

        bool RunNext() { return RunAtMost(1) == 1; }

        size_t Run();

        bool IsEmpty() const { return queue_.IsEmpty(); }

        bool NonEmpty() const { return !IsEmpty(); }

    private:
        IntrusiveForwardList<IntrusiveTask> queue_;
    };

}  // namespace DummyConcurrency::Scheduler
