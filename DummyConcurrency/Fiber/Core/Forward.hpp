#pragma once

#include "DummyConcurrency/Runtime/Scheduler/Scheduler.hpp"
#include "DummyConcurrency/Runtime/Scheduler/Task.hpp"

namespace DummyConcurrency::Fiber {

    class ICoroutine;
    template <typename F> class Coroutine;

    class Fiber;
    class IAwaiter;

    using Runtime::IScheduler;
    using Runtime::ITask;

}  // namespace DummyConcurrency::Fiber
