#pragma once

#include "DummyConcurrency/DataStructures/AtomicSharedPtr.hpp"        // IWYU pragma: export
#include "DummyConcurrency/DataStructures/LockFreeStack.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Go.hpp"                   // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Yield.hpp"                // IWYU pragma: export
#include "DummyConcurrency/Scheduler/RunLoop.hpp"                     // IWYU pragma: export
#include "DummyConcurrency/Scheduler/Submit.hpp"                      // IWYU pragma: export
#include "DummyConcurrency/Scheduler/ThreadPool.hpp"                  // IWYU pragma: export
#include "DummyConcurrency/Synchronization/FiberAware/Event.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Synchronization/FiberAware/Mutex.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Synchronization/FiberAware/WaitGroup.hpp"  // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Thread/Event.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Thread/SpinLock.hpp"       // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Thread/WaitGroup.hpp"      // IWYU pragma: export

namespace DummyConcurrency {

    using Scheduler::RunLoop;
    using Scheduler::ThreadPool;

    using FMutex     = Synchronization::FiberAware::Mutex;
    using FEvent     = Synchronization::FiberAware::Event;
    using FWaitGroup = Synchronization::FiberAware::WaitGroup;

    using TSpinLock  = Synchronization::Thread::SpinLock;
    using TWaitGroup = Synchronization::Thread::WaitGroup;
    using TEvent     = Synchronization::Thread::Event;

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