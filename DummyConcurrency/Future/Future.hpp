#pragma once

#include <DummyConcurrency/Future/Result/Result.hpp>
#include <DummyConcurrency/Future/State/ConsumerContract.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <cassert>

namespace DummyConcurrency::Future {

    template <typename T>
    class [[nodiscard]] Future : public NonCopyable {
    public:
        using State     = State::IConsumerContract<T>;
        using ValueType = T;

        explicit Future(State* state, Runtime::IScheduler& scheduler) : state_(state), scheduler_(&scheduler) {}

        Future(Future&& other) : state_(other.state_), scheduler_(other.scheduler_) {
            other.state_     = nullptr;
            other.scheduler_ = nullptr;
        }
        Future& operator=(Future&&) = delete;
        ~Future() { DC_ASSERT(state_ == nullptr, "Future was not consumed"); }

        // One-shot
        void Consume(Callback<T> callback, Runtime::IScheduler& scheduler) && {
            DC_ASSERT(state_ != nullptr, "Future was already consumed");
            state_->SetCallback(std::move(callback), scheduler);
            state_ = nullptr;
        }

        bool IsReady() const { return state_->IsReady(); }

        Runtime::IScheduler& GetScheduler() const { return *scheduler_; }
        void                 SetScheduler(Runtime::IScheduler& scheduler) {
            DC_ASSERT(state_ != nullptr, "Future was already consumed");
            scheduler_ = &scheduler;
        }

    private:
        State*               state_     = nullptr;
        Runtime::IScheduler* scheduler_ = nullptr;
    };

    template <typename T>
    using TryFuture = Future<Result<T>>;

    namespace Traits {
        namespace Detail {
            template <typename T>
            struct ValueOf;

            template <typename T>
            struct ValueOf<Future<T>> {
                using Type = T;
            };
        }  // namespace Detail

        template <typename Future>
        using ValueOf = typename Detail::ValueOf<Future>::Type;
    }  // namespace Traits

}  // namespace DummyConcurrency::Future
