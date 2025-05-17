#pragma once

#include "Coroutine.hpp"
#include "Forward.hpp"
#include "Hint.hpp"

namespace NDummyConcurrency::NFiber {

    class Fiber : public ITask {
    public:
        template <typename F>
        static Fiber* Create(IScheduler& schedule, F&& body, Hint hint);
        static Fiber* Self();
        static bool   ContextSwitchAwailable();

    public:
        virtual void Run() noexcept;

        void Schedule();
        void Suspend(IAwaiter& awaiter);
        void SwitchTo(Fiber& other);

        IScheduler& GetScheduler() const;

    private:
        Fiber(IScheduler* scheduler, ICoroutine* coroutine, LeasedStack&& stack);
        void Destroy();

    private:
        IAwaiter*   suspend_awaiter_ = nullptr;
        IScheduler* scheduler_       = nullptr;
        ICoroutine* coroutine_       = nullptr;
        LeasedStack stack_;
    };

    template <typename F>
    Fiber* Fiber::Create(IScheduler& scheduler, F&& body, Hint hint) {
        DC_PROFILE_SCOPE();

        class Coroutine : public ICoroutine {
        public:
            explicit Coroutine(F&& body, NContext::StackView view) : ICoroutine(view), body_(std::move(body)) {}
            virtual void Body() override { body_(); }

        private:
            F body_;
        };

        LeasedStack stack = hint.StackProvider->GetStack();

        void*      coroutine_address = stack->PreAllocate<Coroutine>();
        void*      fiber_address     = stack->PreAllocate<Fiber>();
        Coroutine* coroutine         = new (coroutine_address) Coroutine(std::move(body), stack->View());
        coroutine->GetFiber()->SetName(hint.Name, hint.Group);
        return new (fiber_address) Fiber(&scheduler, coroutine, std::move(stack));
    }

}  // namespace NDummyConcurrency::NFiber
