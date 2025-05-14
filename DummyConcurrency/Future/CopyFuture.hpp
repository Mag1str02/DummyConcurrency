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
        explicit CopyFuture(Future<T>&& source) : state_(std::make_shared<State>()) {
            std::move(source).Consume([state = state_](T&& value) {
                std::lock_guard guard(state->Lock());
                state->Value.Construct(std::move(value));
                for (auto&& promise : state->Consumers) {
                    std::move(promise).SetValue(promise);
                }
                state->Consumers.clear();
            }, NRuntime::InlineScheduler());
        }

        operator Future<T>() {  // NOLINT
            std::lock_guard guard(state_->Lock());
            if (state_->Value.IsConstructed()) {
                return Ready(*state_->Value);
            } else {
                auto [future, promise] = Contract<T>();
                state_->Consumers.emplace_back(std::move(promise));
                return future;
            }
        }

    private:
        std::shared_ptr<State> state_;
    };
}  // namespace NDummyConcurrency::NFuture