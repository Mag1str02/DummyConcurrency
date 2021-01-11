#pragma once

#include "DummyConcurrency/Scheduler/Interface/Scheduler.hpp"
#include "DummyConcurrency/Scheduler/Interface/Task.hpp"

namespace DummyConcurrency::Scheduler {

    template <typename F> class SubmitTask : public IntrusiveTask {
    public:
        static IntrusiveTask* Create(F function) { return new SubmitTask(std::move(function)); }

        virtual void Run() noexcept {
            function_();
            delete this;
        }

    private:
        explicit SubmitTask(F function) : function_(std::move(function)) {}
        F function_;
    };

    template <typename F> void Submit(IScheduler& scheduler, F function) {
        scheduler.Submit(SubmitTask<F>::Create(std::move(function)));
    }

}  // namespace DummyConcurrency::Scheduler
