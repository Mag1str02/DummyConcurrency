#pragma once

#include <DummyConcurrency/Future/State/Callback.hpp>

#include <DummyConcurrency/Scheduler/Interface/Scheduler.hpp>

namespace DummyConcurrency::Future::State {

    template <typename T>
    class IConsumerContract {
    public:
        virtual ~IConsumerContract()                                                     = default;
        virtual void SetCallback(Callback<T> callback, Scheduler::IScheduler& scheduler) = 0;
    };

};  // namespace DummyConcurrency::Future::State
