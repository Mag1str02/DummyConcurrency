#pragma once

#include "DummyConcurrency/Scheduler/Interface/Scheduler.hpp"
#include "DummyConcurrency/Scheduler/Interface/Task.hpp"

namespace DummyConcurrency::Fiber {

    class ICoroutine;
    template <typename F> class Coroutine;

    class Fiber;
    class IAwaiter;

    using Scheduler::IntrusiveTask;
    using Scheduler::IScheduler;

}  // namespace DummyConcurrency::Fiber
