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
    Scheduler/Inline.cpp
    Scheduler/ThreadPool.cpp
    Scheduler/RunLoop.cpp
    Synchronization/Awaiters/FiberAwaiter.cpp
    Synchronization/FiberAware/Event.cpp
    Synchronization/FiberAware/Mutex.cpp
    Synchronization/FiberAware/WaitGroup.cpp
    Synchronization/Thread/Event.cpp
    Synchronization/Thread/WaitGroup.cpp
    Synchronization/Thread/SpinLock.cpp
)

if (UNIX)
    set(DummyConcurrencySources ${DummyConcurrencySources} Context/Stack/MMap.cpp)
endif()