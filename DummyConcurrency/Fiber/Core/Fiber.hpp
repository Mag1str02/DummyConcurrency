#pragma once

#include "Coroutine.hpp"
#include "Forward.hpp"
#include "Hint.hpp"

namespace DummyConcurrency::Fiber {

    class Fiber : public ITask {
    public:
        template <typename F> static Fiber* Create(IScheduler& schedule, F&& body, Hint hint);
        static Fiber*                       Self();

    public:
        virtual ~Fiber() = default;
        virtual void Run() noexcept;

        void Suspend(IAwaiter& awaiter);
        void Schedule();
        void SwitchTo(Fiber& other);

        IScheduler& GetScheduler() const;

    private:
        Fiber(IScheduler& scheduler, ICoroutine& coroutine);

    private:
        IScheduler& scheduler_;
        ICoroutine& coroutine_;  // Faster to store pointer to coroutine then using
                                 // virtual method to get it each time

        IAwaiter* suspend_awaiter_ = nullptr;
    };

    template <typename F> Fiber* Fiber::Create(IScheduler& scheduler, F&& body, Hint hint) {
        struct FiberContainer : public Fiber {
            Coroutine<F> Coro;

            FiberContainer(IScheduler& scheduler, F&& body, Hint hint) : Fiber(scheduler, Coro), Coro(std::move(body), hint.StackeSize) {}
        };

        return new FiberContainer(scheduler, std::move(body), hint);
    }

}  // namespace DummyConcurrency::Fiber
