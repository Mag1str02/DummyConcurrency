#pragma once

#include "Task.hpp"

namespace DummyConcurrency::Scheduler {

    struct IScheduler {
        virtual ~IScheduler() = default;

        virtual void Submit(IntrusiveTask*) = 0;
    };

}  // namespace DummyConcurrency::Scheduler
