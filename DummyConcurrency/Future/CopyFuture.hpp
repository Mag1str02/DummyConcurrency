#pragma once

#include "Future.hpp"
#include "Make.hpp"

#include <DummyConcurrency/Synchronization/Thread/SpinLock.hpp>
#include <memory>

namespace NDummyConcurrency::NFuture {

    template <std::copy_constructible T>
    class CopyFuture {
    private:
        struct State {
            NSynchronization::NThread::SpinLock Lock;
            ManualLifetime<T>                   Value;
            std::vector<Promise<T>>             Consumers;
        };

    public:
        CopyFuture() = default;
        explicit CopyFuture(Future<T>&& source) : state_(std::make_shared<State>()) {
            std::move(source).Consume([state = state_](T&& value) {
                std::lock_guard guard(state->Lock);
                for (auto&& promise : state->Consumers) {
                    std::move(promise).Set(value);
                }
                state->Value.Construct(std::move(value));
                state->Consumers.clear();
            }, NRuntime::InlineScheduler());
        }

        Future<T> Copy() {
            DC_ASSERT(state_, "Invalid copy future");
            std::lock_guard guard(state_->Lock);
            if (state_->Value.Constructed()) {
                return Ready(*(state_->Value.Get()));
            }
            auto [future, promise] = Contract<T>();
            state_->Consumers.emplace_back(std::move(promise));
            return std::move(future);
        }

        operator Future<T>() { return Copy(); }  // NOLINT

    private:
        std::shared_ptr<State> state_;
    };
}  // namespace NDummyConcurrency::NFuture