#include "Fiber.hpp"

#include "Awaiter.hpp"
#include "Coroutine.hpp"
#include "Handle.hpp"

#include <twist/ed/static/thread_local/ptr.hpp>

namespace DummyConcurrency::Fiber {

    TWISTED_STATIC_THREAD_LOCAL_PTR(Fiber, kCurrentFiber);

    Fiber::Fiber(IScheduler& sched, ICoroutine& coro) : scheduler_(sched), coroutine_(coro) {}

    Fiber* Fiber::Self() {
        return kCurrentFiber;
    }

    IScheduler& Fiber::GetScheduler() const {
        return scheduler_;
    }

    void Fiber::Suspend(IAwaiter& awaiter) {
        DC_ASSERT(kCurrentFiber == this, "Suspend can only be called on active fiber");

        suspend_awaiter_ = &awaiter;
        coroutine_.Suspend();
    }

    void Fiber::Run() noexcept {
        suspend_awaiter_ = nullptr;
        kCurrentFiber    = this;

        coroutine_.Resume();

        // It is possible to other fiber to leave from resume
        Fiber* returned_fiber = kCurrentFiber;
        kCurrentFiber         = nullptr;
        if (returned_fiber->coroutine_.IsCompleted()) {
            delete returned_fiber;
        } else {
            returned_fiber->suspend_awaiter_->Suspend(FiberHandle(returned_fiber));
        }
    }

    void Fiber::SwitchTo(Fiber& other) {
        DC_ASSERT(kCurrentFiber == this, "SwitchTo can only be called on active fiber");

        kCurrentFiber = &other;
        coroutine_.SwitchTo(other.coroutine_);
    }
    void Fiber::Schedule() {
        scheduler_.Submit(this);
    }

}  // namespace DummyConcurrency::Fiber
