#include "Event.hpp"

#include "DummyConcurrency/Fiber/Scheduling/Suspend.hpp"

namespace DummyConcurrency::Fiber {

    void Event::Wait() {
        // Awoid context switch if already fired
        if (queue_head_.load() == kFired) {
            return;
        }
        Awaiter awaiter(this);
        Suspend(awaiter);
    }

    void Event::Fire() {
        Awaiter* head = queue_head_.exchange(kFired);
        while (head != kNotFired) {
            Awaiter* next = head->Next;
            head->Schedule();
            head = next;
        }
    }

    Event::Awaiter::Awaiter(Event* event) : event_(event) {}
    void Event::Awaiter::OnSuspend() noexcept {
        Awaiter* expected_head = event_->queue_head_.load();
        do {
            if (expected_head == kFired) {
                Schedule();
                return;
            }
            Next = expected_head;
        } while (!event_->queue_head_.compare_exchange_strong(expected_head, this));
    }

}  // namespace DummyConcurrency::Fiber