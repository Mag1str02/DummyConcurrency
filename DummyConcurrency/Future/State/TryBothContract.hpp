#pragma once

#include "DummyConcurrency/Future/State/ConsumerContract.hpp"
#include "DummyConcurrency/Future/State/OneAllStateMachine.hpp"

#include "DummyConcurrency/Future/Result/Make.hpp"
#include "DummyConcurrency/Utils/ManualLifetime.hpp"

namespace DummyConcurrency::Future::State {

    template <typename A, typename B>
    class TryBothContract : public Runtime::ITask, public State::IConsumerContract<Result<std::tuple<A, B>>> {
    public:
        static TryBothContract* Create() { return new TryBothContract(); }

        void SetResultA(Result<A> result) {
            bool has_value = result.has_value();
            value_a_.Construct(std::move(result));
            OneAllStateMachine::Result res;
            if (has_value) {
                res = state_.ProduceAll();
            } else {
                error_result_.store(1);
                res = state_.ProduceOne();
            }

            if (res == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
            if (res == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }
        void SetResultB(Result<B> result) {
            bool has_value = result.has_value();
            value_b_.Construct(std::move(result));
            OneAllStateMachine::Result res;
            if (has_value) {
                res = state_.ProduceAll();
            } else {
                error_result_.store(2);
                res = state_.ProduceOne();
            }

            if (res == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
            if (res == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

        virtual void SetCallback(Callback<Result<std::tuple<A, B>>> callback, Runtime::IScheduler& scheduler) override {
            callback_.Construct(std::move(callback));
            scheduler_ = &scheduler;
            if (state_.Consume() == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
        }
        virtual bool IsReady() const override { return state_.IsReady(); }

        virtual void Run() noexcept override {
            auto error_result = error_result_.load();
            if (error_result != 0) {
                (*callback_.Get())(ResultError(error_result == 1 ? value_a_->error() : value_b_->error()));
            } else {
                (*callback_.Get())(std::tuple<A, B>(std::move(value_a_->value()), std::move(value_b_->value())));
            }
            if (state_.Finish() == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

    private:
        TryBothContract() : state_(2) {}

        void SubmitContract() { scheduler_->Submit(this); }

    private:
        OneAllStateMachine state_;

        ManualLifetime<Result<A>>             value_a_;
        ManualLifetime<Result<B>>             value_b_;
        ImplementationLayer::Atomic<uint32_t> error_result_ = 0;  // kill me...

        ManualLifetime<Callback<Result<std::tuple<A, B>>>> callback_;
        Runtime::IScheduler*                               scheduler_;
    };

}  // namespace DummyConcurrency::Future::State