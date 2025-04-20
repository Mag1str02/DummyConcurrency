#pragma once

#include "DummyConcurrency/Future/State/ConsumerContract.hpp"
#include "DummyConcurrency/Future/State/OneAllStateMachine.hpp"

#include "DummyConcurrency/Util/ManualLifetime.hpp"

namespace DummyConcurrency::Future::State {

    template <typename A, typename B>
    class BothContract : public sched::task::TaskBase, public state::IConsumerContract<std::tuple<A, B>> {
    public:
        static BothContract* Create() { return new BothContract(); }

        void SetValueA(A value) {
            value_a_.Construct(std::move(value));
            auto res = state_.ProduceAll();

            if (res == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
            if (res == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }
        void SetValueB(B value) {
            value_b_.Construct(std::move(value));
            auto res = state_.ProduceAll();

            if (res == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
            if (res == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

        virtual void SetCallback(Callback<std::tuple<A, B>> callback, sched::task::IScheduler& scheduler) override {
            callback_.Construct(std::move(callback));
            scheduler_ = &scheduler;
            if (state_.Consume() == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
        }

        virtual void Run() noexcept override {
            (*callback_.Get())(std::tuple<A, B>(value_a_.Move(), value_b_.Move()));
            if (state_.Finish() == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

    private:
        BothContract() : state_(2) {}

        void SubmitContract() { scheduler_->Submit(this); }

    private:
        OneAllStateMachine state_;

        ManualLifetime<A> value_a_;
        ManualLifetime<B> value_b_;

        ManualLifetime<Callback<std::tuple<A, B>>> callback_;
        sched::task::IScheduler*                   scheduler_;
    };

}  // namespace DummyConcurrency::Future::State