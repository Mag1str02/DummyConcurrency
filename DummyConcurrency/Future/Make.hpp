#pragma once

#include <DummyConcurrency/Fiber/Scheduling/Go.hpp>
#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Result/Make.hpp>
#include <DummyConcurrency/Result/Traits.hpp>

namespace NDummyConcurrency::NFuture {

    template <typename T>
    Future<T> Ready(T value) {
        auto* contract = NState::ContractState<T>::Create();
        contract->SetValue(std::move(value));
        return Future<T>(contract, NRuntime::InlineScheduler());
    }

    template <typename T>
    Future<T> Return(T value) {
        return Ready(std::move(value));
    }

    template <typename T>
    Future<T> Value(T v)
        requires NResult::CNotResult<T>
    {
        return Ready(std::move(v));
    }

    inline Future<Unit> Just() {
        return Value(Unit());
    }

    namespace NDetail {
        struct [[nodiscard]] Failure {
            Error e;  // NOLINT

            template <typename T>
            operator TryFuture<T>() const {  // NOLINT
                return Ready<Result<T>>(NResult::Failure(e));
            }
        };
    }  // namespace NDetail

    template <typename T>
    TryFuture<T> Ok(T value) {
        return Ready(NResult::Ok(std::move(value)));
    }
    inline auto Failure(Error e) {
        return NDetail::Failure{e};
    }

    template <typename T>
    class Promise {
    public:
        using State = NState::ContractState<T>;

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
        auto* state = NState::ContractState<T>::Create();
        return {Future<T>(state, NRuntime::InlineScheduler()), Promise<T>(state)};
    }

    template <typename F>
    Future<std::invoke_result_t<F>> Submit(NRuntime::IScheduler& scheduler, F function) {
        using Value                            = std::invoke_result_t<F>;
        NState::ContractState<Value>* contract = NState::ContractState<Value>::Create();
        ::NDummyConcurrency::NRuntime::Submit(scheduler, [contract, func = std::move(function)]() { contract->SetValue(func()); });

        return Future<Value>(contract, NRuntime::InlineScheduler());
    }
    template <typename F>
    Future<std::invoke_result_t<F>> Go(NRuntime::IScheduler& scheduler, F function, NFiber::Hint = {}) {
        using Value                            = std::invoke_result_t<F>;
        NState::ContractState<Value>* contract = NState::ContractState<Value>::Create();
        NFiber::Go(scheduler, [contract, func = std::move(function)]() { contract->SetValue(func()); });
        return Future<Value>(contract, NRuntime::InlineScheduler());
    }
}  // namespace NDummyConcurrency::NFuture
