#pragma once

#include "Task.hpp"

namespace DummyConcurrency::Runtime {

    struct IScheduler {
        virtual ~IScheduler() = default;

        virtual void Submit(ITask*) = 0;
    };

}  // namespace DummyConcurrency::Runtime
