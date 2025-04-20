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
    Fiber/Synchronization/Event.cpp
    Fiber/Synchronization/Mutex.cpp
    Fiber/Synchronization/WaitGroup.cpp
    Scheduler/Inline.cpp
    Scheduler/ThreadPool.cpp
    Scheduler/RunLoop.cpp
    Synchronization/Event.cpp
    Synchronization/WaitGroup.cpp
    Synchronization/SpinLock.cpp
)

if (UNIX)
    set(DummyConcurrencySources ${DummyConcurrencySources} Context/Stack/MMap.cpp)
endif()