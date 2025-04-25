#pragma once

#include "DummyConcurrency/DataStructures/AtomicSharedPtr.hpp"        // IWYU pragma: export
#include "DummyConcurrency/DataStructures/LockFreeStack.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Go.hpp"                   // IWYU pragma: export
#include "DummyConcurrency/Fiber/Scheduling/Yield.hpp"                // IWYU pragma: export
#include "DummyConcurrency/Fiber/Stack/StackPool.hpp"                 // IWYU pragma: export
#include "DummyConcurrency/Runtime/Scheduler/RunLoop.hpp"             // IWYU pragma: export
#include "DummyConcurrency/Runtime/Scheduler/Submit.hpp"              // IWYU pragma: export
#include "DummyConcurrency/Runtime/Scheduler/ThreadPool.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Synchronization/FiberAware/Event.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Synchronization/FiberAware/Mutex.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Synchronization/FiberAware/WaitGroup.hpp"  // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Thread/Event.hpp"          // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Thread/SpinLock.hpp"       // IWYU pragma: export
#include "DummyConcurrency/Synchronization/Thread/WaitGroup.hpp"      // IWYU pragma: export
#include "DummyConcurrency/Syntax/Syntax.hpp"                         // IWYU pragma: export

namespace NDummyConcurrency {

    using NFiber::StackPool;
    using NRuntime::RunLoop;
    using NRuntime::ThreadPool;

    using FMutex     = NSynchronization::NFiberAware::Mutex;
    using FEvent     = NSynchronization::NFiberAware::Event;
    using FWaitGroup = NSynchronization::NFiberAware::WaitGroup;

    using TSpinLock  = NSynchronization::NThread::SpinLock;
    using TWaitGroup = NSynchronization::NThread::WaitGroup;
    using TEvent     = NSynchronization::NThread::Event;

    template <typename T>
    using AtomicSharedPtr = NDataStructures::AtomicSharedPtr<T>;
    template <typename T>
    using SharedPtr = NDataStructures::SharedPtr<T>;
    template <typename T>
    using LockFreeStack = NDataStructures::LockFreeStack<T>;
    using NDataStructures::MakeShared;

    using NFiber::Go;
    using NFiber::Yield;
    using NRuntime::Submit;

}  // namespace NDummyConcurrency