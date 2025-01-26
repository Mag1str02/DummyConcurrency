#pragma once

#ifdef DC_USE_TWIST

#include <twist/assist/fiber.hpp>
#include <twist/ed/static/thread_local/ptr.hpp>
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/condition_variable.hpp>
#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/wait/futex.hpp>
#include <twist/ed/wait/spin.hpp>

namespace DummyConcurrency::ImplementationLayer {

    template <typename T> using Atomic = twist::ed::std::atomic<T>;
    using Thread                       = twist::ed::std::thread;
    using Mutex                        = twist::ed::std::mutex;
    using ConditionVariable            = twist::ed::std::condition_variable;
    using SpinWait                     = twist::ed::SpinWait;
    using Fiber                        = twist::assist::Fiber;
    using FiberHandle                  = twist::assist::FiberHandle;
    using twist::assist::SwitchToFiber;
    using twist::assist::NewFiber;

    namespace Futex = twist::ed::futex;

#define STATIC_THREAD_LOCAL_PTR(type, name) TWISTED_STATIC_THREAD_LOCAL_PTR(type, name);
}  // namespace DummyConcurrency::ImplementationLayer

#else

namespace DummyConcurrency::ImplementationLayer {
    using namespace std;
}

#endif
