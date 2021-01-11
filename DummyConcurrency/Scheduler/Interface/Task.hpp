#pragma once

#include "DummyConcurrency/Utils/IntrusiveForwardList.hpp"

namespace DummyConcurrency::Scheduler {

    struct ITask {
        virtual ~ITask() = default;

        virtual void Run() noexcept = 0;
    };

    struct IntrusiveTask : ITask, IntrusiveForwardListNode<IntrusiveTask> {};

}  // namespace DummyConcurrency::Scheduler
