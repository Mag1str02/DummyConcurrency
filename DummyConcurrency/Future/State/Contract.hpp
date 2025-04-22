#pragma once

#include <DummyConcurrency/Future/State/Callback.hpp>
#include <DummyConcurrency/Future/State/ConsumerContract.hpp>
#include <DummyConcurrency/Future/State/Rendezvous.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Inline.hpp>
#include <DummyConcurrency/Runtime/Scheduler/Submit.hpp>
#include <DummyConcurrency/Utils/ManualLifetime.hpp>

namespace NDummyConcurrency::NFuture::NState {

    template <typename T>
    class ContractState : public NRuntime::ITask, public IConsumerContract<T> {
    public:
        static ContractState* Create() { return new ContractState(); }

        void SetValue(T value) {
            value_.Construct(std::move(value));
            if (state_.Produce()) {
                SubmitContract();
            }
        }

        virtual void SetCallback(Callback<T> callback, NRuntime::IScheduler& scheduler) override {
            callback_.Construct(std::move(callback));
            scheduler_ = &scheduler;
            if (state_.Consume()) {
                SubmitContract();
            }
        }

        virtual bool IsReady() const override { return state_.IsReady(); }

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
        NRuntime::IScheduler*        scheduler_;
    };

}  // namespace NDummyConcurrency::NFuture::NState
