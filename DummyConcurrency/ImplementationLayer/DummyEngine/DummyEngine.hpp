#pragma once

#include "Fiber.hpp"

#include <DummyConcurrency/ImplementationLayer/STD/Futex.hpp>
#include <DummyConcurrency/ImplementationLayer/STD/SpinWait.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

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
    using Fiber                        = NDummyEngine::Fiber;
    using FiberHandle                  = NDummyEngine::FiberHandle;
    using NDummyEngine::NewFiber;
    using NDummyEngine::SwitchToFiber;

    namespace Futex {
        using STD::PrepareWake;
        using STD::Wait;
        using STD::WakeAll;
        using STD::WakeOne;
    }  // namespace Futex

#define STATIC_THREAD_LOCAL_PTR(type, name) static thread_local type* name = nullptr;

}  // namespace NDummyConcurrency::NImplementationLayer