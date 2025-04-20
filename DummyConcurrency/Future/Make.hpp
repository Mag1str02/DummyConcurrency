#pragma once

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/Result/Make.hpp>
#include <DummyConcurrency/Future/Result/Traits.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>

namespace DummyConcurrency::Future {

    template <typename T>
    Future<T> Ready(T value) {
        auto* contract = State::ContractState<T>::Create();
        contract->SetValue(std::move(value));
        return Future<T>(contract, Scheduler::Inline());
    }

    template <typename T>
    Future<T> Return(T value) {
        return Ready(std::move(value));
    }

    template <typename T>
    Future<T> Value(T v) {
        static_assert(!::DummyConcurrency::Traits::IsResult<T>, "Expected plain value");
        return Ready(std::move(v));
    }

    inline Future<Unit> Just() {
        return Value(Unit());
    }

    template <typename T>
    TryFuture<T> Ok(T value) {
        return Ready(ResultOk(std::move(value)));
    }
    struct [[nodiscard]] Failure {
        Error e;  // NOLINT

        template <typename T>
        operator TryFuture<T>() const {  // NOLINT
            return Ready<Result<T>>(ResultError(e));
        }
    };

    inline auto Error(Error e) {
        return Failure{e};
    }

    /*
     * Usage:
     *
     * future::TryFuture<int> Attempt() {
     *   return future::Err(TimeoutError());
     * }
     *
     */

    template <typename T>
    class Promise {
    public:
        using State = State::ContractState<T>;

        explicit Promise(State* state) : state_(state) {}

        // Move-constructible
        Promise(Promise&& other) : state_(other.state_) { other.state_ = nullptr; }

        // Non-copyable
        Promise(const Promise&)            = delete;
        Promise& operator=(const Promise&) = delete;

        // Non-move-assignable
        Promise& operator=(Promise&&) = delete;

        ~Promise() { DC_ASSERT(state_ == nullptr, "Promise was not fulfilled"); }

        // One-shot
        void Set(T value) && {
            DC_ASSERT(state_ != nullptr, "Promise was already fulfilled");
            state_->SetValue(std::move(value));
            state_ = nullptr;
        }

    private:
        State* state_ = nullptr;
    };

    template <typename T>
    std::tuple<Future<T>, Promise<T>> Contract() {
        auto* state = State::ContractState<T>::Create();
        return {Future<T>(state, Scheduler::Inline()), Promise<T>(state)};
    }

    template <typename F>
    Future<std::invoke_result_t<F>> Submit(Scheduler::IScheduler& scheduler, F function) {
        using Value                           = std::invoke_result_t<F>;
        State::ContractState<Value>* contract = State::ContractState<Value>::Create();
        ::DummyConcurrency::Scheduler::Submit(scheduler, [contract, func = std::move(function)]() { contract->SetValue(func()); });

        return Future<Value>(contract, Scheduler::Inline());
    }

}  // namespace DummyConcurrency::Future
