#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Future::State {

    namespace {

        static constexpr uint32_t kConsumedBit    = 1 << 0;
        static constexpr uint32_t kFinishedBit    = 1 << 1;
        static constexpr uint32_t kProducedOneBit = 1 << 2;
        static constexpr uint32_t kProducedAllBit = 1 << 3;

        bool HasProducedOne(uint32_t state) {
            return (state & kProducedOneBit) != 0;
        }
        bool HasProducedAll(uint32_t state, uint32_t producer_count) {
            return (state & ~(kConsumedBit | kProducedOneBit | kFinishedBit)) == producer_count * kProducedAllBit;
        }
        bool HasProduced(uint32_t state, uint32_t producer_count) {
            return HasProducedOne(state) || HasProducedAll(state, producer_count);
        }
        bool IsCompleted(uint32_t state, uint32_t producer_count) {
            return HasProducedAll(state, producer_count) && ((state & kFinishedBit) != 0);
        }
        bool HasConsume(uint32_t state) {
            return (state & kConsumedBit) != 0;
        }

    }  // namespace

    class OneAllStateMachine {
    public:
        enum class Result {
            InProgress,
            Rendezvous,
            Finishing,
            Completed,
        };

        explicit OneAllStateMachine(uint32_t producer_count) : producer_count_(producer_count) {}

        Result Consume() {
            uint32_t producer_count = producer_count_;
            uint32_t new_state      = state_.fetch_add(kConsumedBit) + kConsumedBit;
            if (HasConsume(new_state) && HasProduced(new_state, producer_count)) {
                return Result::Rendezvous;
            }
            return Result::InProgress;
        }
        Result Finish() {
            uint32_t producer_count = producer_count_;
            uint32_t new_state      = state_.fetch_add(kFinishedBit) + kFinishedBit;
            if (IsCompleted(new_state, producer_count)) {
                return Result::Completed;
            }
            return Result::Finishing;
        }
        Result ProduceAll() {
            uint32_t producer_count = producer_count_;
            uint32_t new_state      = state_.fetch_add(kProducedAllBit) + kProducedAllBit;
            if (IsCompleted(new_state, producer_count)) {
                return Result::Completed;
            }
            if (HasConsume(new_state) && !HasProducedOne(new_state) && HasProducedAll(new_state, producer_count)) {
                return Result::Rendezvous;
            }
            if (!HasConsume(new_state)) {
                return Result::InProgress;
            }
            return Result::Finishing;
        }
        Result ProduceOne() {
            uint32_t producer_count = producer_count_;
            uint32_t new_state      = state_.load();
            do {
            } while (!state_.compare_exchange_weak(new_state, (new_state | kProducedOneBit) + kProducedAllBit));
            uint32_t old_state = new_state;
            new_state          = (new_state | kProducedOneBit) + kProducedAllBit;
            if (!HasConsume(new_state)) {
                return Result::InProgress;
            }
            if (!HasProducedOne(old_state)) {
                return Result::Rendezvous;
            }
            if (IsCompleted(new_state, producer_count)) {
                return Result::Completed;
            }
            return Result::Finishing;
        }
        bool IsReady() const { return HasProduced(state_.load(), producer_count_); }

    private:
        ImplementationLayer::Atomic<uint32_t> state_ = 0;
        const uint32_t                        producer_count_;
    };

}  // namespace DummyConcurrency::Future::State