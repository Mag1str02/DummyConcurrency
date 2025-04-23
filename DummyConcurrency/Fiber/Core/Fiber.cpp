#include "Fiber.hpp"

#include "Awaiter.hpp"
#include "Coroutine.hpp"
#include "Handle.hpp"

#include "DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp"

namespace NDummyConcurrency::NFiber {

    STATIC_THREAD_LOCAL_PTR(Fiber, gCurrentFiber);

    Fiber::Fiber(IScheduler* scheduler, ICoroutine* coroutine, LeasedStack&& stack) :
        scheduler_(scheduler), coroutine_(coroutine), stack_(std::move(stack)) {}

    void Fiber::Destroy() {
        Fiber*      fiber     = this;
        ICoroutine* coroutine = coroutine_;
        LeasedStack stack     = std::move(stack_);

        coroutine->~ICoroutine();
        fiber->~Fiber();
    }

    Fiber* Fiber::Self() {
        return gCurrentFiber;
    }

    bool Fiber::ContextSwitchAwailable() {
        return gCurrentFiber != nullptr && NContext::ExceptionsContext::IsSwitchAllowed();
    }

    IScheduler& Fiber::GetScheduler() const {
        return *scheduler_;
    }

    void Fiber::Suspend(IAwaiter& awaiter) {
        DC_ASSERT(gCurrentFiber == this, "Suspend can only be called on active fiber");

        suspend_awaiter_ = &awaiter;
        coroutine_->Suspend();
    }

    void Fiber::Run() noexcept {
        suspend_awaiter_ = nullptr;
        gCurrentFiber    = this;

        coroutine_->Resume();

        // It is possible to other fiber to leave from resume
        Fiber* returned_fiber = gCurrentFiber;
        gCurrentFiber         = nullptr;
        if (returned_fiber->coroutine_->IsCompleted()) {
            returned_fiber->Destroy();
        } else {
            returned_fiber->suspend_awaiter_->Suspend(Handle(returned_fiber));
        }
    }

    void Fiber::SwitchTo(Fiber& other) {
        DC_ASSERT(gCurrentFiber == this, "SwitchTo can only be called on active fiber");

        gCurrentFiber = &other;
        coroutine_->SwitchTo(*other.coroutine_);
    }
    void Fiber::Schedule() {
        scheduler_->Submit(this);
    }

}  // namespace NDummyConcurrency::NFiber
