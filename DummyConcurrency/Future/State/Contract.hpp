#pragma once

#include <DummyConcurrency/Future/State/Callback.hpp>
#include <DummyConcurrency/Future/State/ConsumerContract.hpp>
#include <DummyConcurrency/Future/State/Rendezvous.hpp>

#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Submit.hpp>
#include <DummyConcurrency/Utils/ManualLifetime.hpp>

namespace DummyConcurrency::Future::State {

    template <typename T>
    class ContractState : public Runtime::ITask, public IConsumerContract<T> {
    public:
        static ContractState* Create() { return new ContractState(); }

        void SetValue(T value) {
            value_.Construct(std::move(value));
            if (state_.Produce()) {
                SubmitContract();
            }
        }

        virtual void SetCallback(Callback<T> callback, Runtime::IScheduler& scheduler) override {
            callback_.Construct(std::move(callback));
            scheduler_ = &scheduler;
            if (state_.Consume()) {
                SubmitContract();
            }
        }

        virtual void Run() noexcept override {
            (*callback_.Get())(value_.Move());
            delete this;
        }

    private:
        ContractState() = default;
        void SubmitContract() { scheduler_->Submit(this); }

    private:
        RendezvousStateMachine      state_;
        ManualLifetime<T>           value_;
        ManualLifetime<Callback<T>> callback_;
        Runtime::IScheduler*      scheduler_;
    };

}  // namespace DummyConcurrency::Future::State
