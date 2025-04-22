#pragma once

#include <DummyConcurrency/Future/State/Callback.hpp>

#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>

namespace DummyConcurrency::Future::State {

    template <typename T>
    class IConsumerContract {
    public:
        virtual ~IConsumerContract()                                                   = default;
        virtual void SetCallback(Callback<T> callback, Runtime::IScheduler& scheduler) = 0;
        virtual bool IsReady() const                                                   = 0;
    };

};  // namespace DummyConcurrency::Future::State
