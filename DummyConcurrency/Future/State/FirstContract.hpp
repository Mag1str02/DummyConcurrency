#pragma once

#include "DummyConcurrency/Future/State/ConsumerContract.hpp"
#include "DummyConcurrency/Future/State/OneAllStateMachine.hpp"

#include "DummyConcurrency/Utils/ManualLifetime.hpp"

namespace DummyConcurrency::Future::State {

    template <typename T>
    class PlainFirstContract : public Scheduler::ITask, public State::IConsumerContract<T> {
    public:
        static PlainFirstContract* Create(uint32_t producer_count) { return new PlainFirstContract(producer_count); }

        void SetValue(T value) {
            SetValuePtr(std::move(value));
            auto res = state_.ProduceOne();

            if (res == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
            if (res == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

        virtual void SetCallback(Callback<T> callback, Scheduler::IScheduler& scheduler) override {
            callback_.Construct(std::move(callback));
            scheduler_ = &scheduler;
            if (state_.Consume() == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
        }

        virtual void Run() noexcept override {
            T* value = value_.load();
            (*callback_.Get())(*value);
            delete value;
            if (state_.Finish() == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

    private:
        explicit PlainFirstContract(uint32_t producer_count) : state_(producer_count) {}

        void SubmitContract() { scheduler_->Submit(this); }

        void SetValuePtr(T value) {
            T* ptr = value_.load();
            if (ptr != nullptr) {
                return;
            }
            T* new_value = new T(std::move(value));
            do {
                if (ptr != nullptr) {
                    delete new_value;
                    return;
                }
            } while (!value_.compare_exchange_weak(ptr, new_value));
        };

    private:
        OneAllStateMachine          state_;
        twist::ed::std::atomic<T*>  value_ = nullptr;
        ManualLifetime<Callback<T>> callback_;
        Scheduler::IScheduler*      scheduler_;
    };

}  // namespace DummyConcurrency::Future::State