#pragma once

#include <DummyConcurrency/Future/State/Callback.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>

namespace NDummyConcurrency::NFuture::NState {

    template <typename T>
    class IConsumerContract {
    public:
        virtual ~IConsumerContract()                                                   = default;
        virtual void SetCallback(Callback<T> callback, NRuntime::IScheduler& scheduler) = 0;
        virtual bool IsReady() const                                                   = 0;
    };

};  // namespace NDummyConcurrency::NFuture::NState
