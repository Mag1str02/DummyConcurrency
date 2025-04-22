#pragma once

#include "DummyConcurrency/Runtime/Scheduler/Scheduler.hpp"
#include "DummyConcurrency/Runtime/Scheduler/Task.hpp"

namespace NDummyConcurrency::NFiber {

    class ICoroutine;
    template <typename F> class Coroutine;

    class Fiber;
    class IAwaiter;

    using NRuntime::IScheduler;
    using NRuntime::ITask;

}  // namespace NDummyConcurrency::NFiber
