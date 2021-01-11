#pragma once

#include "DummyConcurrency/Fiber/Scheduling/Go.hpp"                  // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Yield.hpp"               // IWYU pragma: export
#include "DummyConcurrency/Fiber/Synchronization/Event.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Fiber/Synchronization/Mutex.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Fiber/Synchronization/WaitGroup.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Scheduler/RunLoop.hpp"                    // IWYU pragma: export
#include "DummyConcurrency/Scheduler/Submit.hpp"                     // IWYU pragma: export
#include "DummyConcurrency/Scheduler/ThreadPool.hpp"                 // IWYU pragma: export
#include "DummyConcurrency/Threading/Synchronization/SpinLock.hpp"   // IWYU pragma: export
#include "DummyConcurrency/Threading/Synchronization/WaitGroup.hpp"  // IWYU pragma: export

namespace DummyConcurrency {

    using Scheduler::RunLoop;
    using Scheduler::ThreadPool;

    using FMutex     = Fiber::Mutex;
    using FEvent     = Fiber::Event;
    using FWaitGroup = Fiber::WaitGroup;

    using TSpinLock  = Threading::SpinLock;
    using TWaitGroup = Threading::WaitGroup;

    using Fiber::Go;
    using Fiber::Yield;
    using Scheduler::Submit;

}  // namespace DummyConcurrency