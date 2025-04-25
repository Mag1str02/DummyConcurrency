#pragma once

#include "Task.hpp"

namespace NDummyConcurrency::NRuntime {

    struct IScheduler {
        virtual ~IScheduler() = default;

        virtual void Submit(ITask* task) = 0;
    };

}  // namespace NDummyConcurrency::NRuntime
