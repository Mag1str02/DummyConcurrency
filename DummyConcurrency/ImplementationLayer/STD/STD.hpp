#pragma once

#include "Fiber.hpp"
#include "Futex.hpp"
#include "SpinWait.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace NDummyConcurrency::NImplementationLayer {

    template <typename T> using Atomic = std::atomic<T>;
    using Thread                       = std::thread;
    using Mutex                        = std::mutex;
    using ConditionVariable            = std::condition_variable;
    using SpinWait                     = STD::SpinWait;
    using Fiber                        = STD::Fiber;
    using FiberHandle                  = STD::FiberHandle;
    using STD::NewFiber;
    using STD::SwitchToFiber;

    namespace Futex {
        using STD::PrepareWake;
        using STD::Wait;
        using STD::WakeAll;
        using STD::WakeOne;
    }  // namespace Futex

#define STATIC_THREAD_LOCAL_PTR(type, name) static thread_local type* name = nullptr;

}  // namespace NDummyConcurrency::NImplementationLayer