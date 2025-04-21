#pragma once

#include "DummyConcurrency/Future/State/ConsumerContract.hpp"
#include "DummyConcurrency/Future/State/OneAllStateMachine.hpp"

#include "DummyConcurrency/Future/Result/Result.hpp"
#include "DummyConcurrency/Utils/ManualLifetime.hpp"

namespace DummyConcurrency::Future::State {

    template <typename T>
    class TryFirstContract : public Runtime::ITask, public State::IConsumerContract<Result<T>> {
    public:
        static TryFirstContract* Create(uint32_t producer_count) { return new TryFirstContract(producer_count); }

        ~TryFirstContract() { DeleteResults(); }

        void SetResult(Result<T> result) {
            bool has_value = result.has_value();
            AddResult(std::move(result));
            OneAllStateMachine::Result res;
            if (has_value) {
                res = state_.ProduceOne();
            } else {
                res = state_.ProduceAll();
            }
            if (res == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
            if (res == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }
        virtual void SetCallback(Callback<Result<T>> callback, Runtime::IScheduler& scheduler) override {
            callback_.Construct(std::move(callback));
            scheduler_ = &scheduler;
            if (state_.Consume() == OneAllStateMachine::Result::Rendezvous) {
                SubmitContract();
            }
        }

        virtual void Run() noexcept override {
            auto* node = one_result_.load();
            if (node == nullptr) {
                node = results_.load();
            }
            (*callback_.Get())(std::move(node->Res));
            if (state_.Finish() == OneAllStateMachine::Result::Completed) {
                delete this;
            }
        }

    private:
        explicit TryFirstContract(uint32_t producer_count) : state_(producer_count) {}

        void SubmitContract() { scheduler_->Submit(this); }

    private:
        struct Node {
            Node*     Next;
            Result<T> Res;

            explicit Node(Result<T> result) : Res(result) {}
        };

        void AddResult(Result<T> result) {
            bool  has_value = result.has_value();
            auto* node      = new Node(result = std::move(result));
            auto* expected  = results_.load();
            do {
                node->Next = expected;
            } while (!results_.compare_exchange_weak(expected, node));
            if (has_value) {
                one_result_.store(node);
            }
        }
        void DeleteResults() {
            auto* node = results_.load();
            while (node != nullptr) {
                auto* next = node->Next;
                delete node;
                node = next;
            }
        }

    private:
        OneAllStateMachine state_;

        ImplementationLayer::Atomic<Node*> results_    = nullptr;
        ImplementationLayer::Atomic<Node*> one_result_ = nullptr;

        ManualLifetime<Callback<Result<T>>> callback_;
        Runtime::IScheduler*              scheduler_;
    };

}  // namespace DummyConcurrency::Future::State