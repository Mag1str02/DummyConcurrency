#pragma once

#include <DummyConcurrency/Future/State/ConsumerContract.hpp>
#include <DummyConcurrency/Result/Result.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

namespace NDummyConcurrency::NFuture {

    template <typename T>
    using Result = NResult::Result<T>;
    using NResult::Error;
    using NResult::Status;
    using NResult::Unit;

    template <std::move_constructible T>
    class [[nodiscard]] Future : public NonCopyable {
    public:
        using State     = NState::IConsumerContract<T>;
        using ValueType = T;

        explicit Future(State* state, NRuntime::IScheduler& scheduler) : state_(state), scheduler_(&scheduler) {}

        Future(Future&& other) : state_(other.state_), scheduler_(other.scheduler_) {
            other.state_     = nullptr;
            other.scheduler_ = nullptr;
        }
        Future& operator=(Future&&) = delete;
        ~Future() { DC_ASSERT(state_ == nullptr, "Future was not consumed"); }

        // One-shot
        void Consume(Callback<T> callback, NRuntime::IScheduler& scheduler) && {
            DC_ASSERT(state_ != nullptr, "Future was already consumed");
            state_->SetCallback(std::move(callback), scheduler);
            state_ = nullptr;
        }

        bool IsReady() const { return state_->IsReady(); }

        NRuntime::IScheduler& GetScheduler() const { return *scheduler_; }
        void                  SetScheduler(NRuntime::IScheduler& scheduler) {
            DC_ASSERT(state_ != nullptr, "Future was already consumed");
            scheduler_ = &scheduler;
        }

    private:
        State*                state_     = nullptr;
        NRuntime::IScheduler* scheduler_ = nullptr;
    };

    template <std::move_constructible T>
    using TryFuture = Future<Result<T>>;

}  // namespace NDummyConcurrency::NFuture
