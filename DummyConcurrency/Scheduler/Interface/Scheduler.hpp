#pragma once

#include "Task.hpp"

namespace DummyConcurrency::Scheduler {

    struct IScheduler {
        virtual ~IScheduler() = default;

        virtual void Submit(ITask*) = 0;
    };

}  // namespace DummyConcurrency::Scheduler
