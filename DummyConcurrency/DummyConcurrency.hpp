#pragma once

#include "DummyConcurrency/DataStructures/AtomicSharedPtr.hpp"   // IWYU pragma: export
#include "DummyConcurrency/DataStructures/LockFreeStack.hpp"     // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Go.hpp"              // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Yield.hpp"           // IWYU pragma: export
#include "DummyConcurrency/Fiber/Synchronization/Event.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Fiber/Synchronization/Mutex.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Fiber/Synchronization/WaitGroup.hpp"  // IWYU pragma: export
#include "DummyConcurrency/Scheduler/RunLoop.hpp"                // IWYU pragma: export
#include "DummyConcurrency/Scheduler/Submit.hpp"                 // IWYU pragma: export
#include "DummyConcurrency/Scheduler/ThreadPool.hpp"             // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Event.hpp"            // IWYU pragma: export
#include "DummyConcurrency/Synchronization/SpinLock.hpp"         // IWYU pragma: export
#include "DummyConcurrency/Synchronization/WaitGroup.hpp"        // IWYU pragma: export

namespace DummyConcurrency {

    using Scheduler::RunLoop;
    using Scheduler::ThreadPool;

    using FMutex     = Fiber::Mutex;
    using FEvent     = Fiber::Event;
    using FWaitGroup = Fiber::WaitGroup;

    using TSpinLock  = SpinLock;
    using TWaitGroup = WaitGroup;
    using TEvent     = Event;

    template <typename T>
    using AtomicSharedPtr = DataStructures::AtomicSharedPtr<T>;
    template <typename T>
    using SharedPtr = DataStructures::SharedPtr<T>;
    template <typename T>
    using LockFreeStack = DataStructures::LockFreeStack<T>;
    using DataStructures::MakeShared;

    using Fiber::Go;
    using Fiber::Yield;
    using Scheduler::Submit;

}  // namespace DummyConcurrency