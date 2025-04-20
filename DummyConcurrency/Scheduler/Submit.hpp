#pragma once

#include "DummyConcurrency/Scheduler/Interface/Scheduler.hpp"
#include "DummyConcurrency/Scheduler/Interface/Task.hpp"

namespace DummyConcurrency::Scheduler {

    template <typename F> 
    void Submit(IScheduler& scheduler, F function) {
        class SubmitTask : public ITask {
        public:
            explicit SubmitTask(F function) : function_(std::move(function)) {}

            virtual void Run() noexcept {
                function_();
                delete this;
            }

        private:
            F function_;
        };

        scheduler.Submit(new SubmitTask(std::move(function)));
    }

}  // namespace DummyConcurrency::Scheduler
