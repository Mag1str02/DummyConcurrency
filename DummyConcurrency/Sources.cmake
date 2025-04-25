set(DummyConcurrencySources
    Context/Context.cpp
    Context/Exceptions/${DC_EXCEPTION_CONTEXT_IMPLEMENTATION}/Context.cpp
    Context/Machine/${DC_MACHINE_CONTEXT_IMPLEMENTATION}/Context.cpp
    Context/Machine/${DC_MACHINE_CONTEXT_IMPLEMENTATION}/Context.S

    Fiber/Core/Coroutine.cpp
    Fiber/Core/Fiber.cpp
    Fiber/Core/Handle.cpp
    Fiber/Scheduling/Yield.cpp
    Fiber/Scheduling/Suspend.cpp
    Fiber/Stack/StackPool.cpp
    Fiber/Stack/StackProvider.cpp

    Runtime/Invoker/Inline.cpp
    Runtime/Scheduler/Inline.cpp
    Runtime/Scheduler/ThreadPool.cpp
    Runtime/Scheduler/RunLoop.cpp

    Synchronization/Awaiters/FiberAwaiter.cpp
    Synchronization/FiberAware/Event.cpp
    Synchronization/FiberAware/Mutex.cpp
    Synchronization/FiberAware/WaitGroup.cpp
    Synchronization/Thread/Event.cpp
    Synchronization/Thread/WaitGroup.cpp
    Synchronization/Thread/SpinLock.cpp
)
