#include "Event.hpp"

#include <DummyConcurrency/Synchronization/Awaiters/DoAwait.hpp>

namespace NDummyConcurrency::NSynchronization::NFiberAware {

    void Event::Wait() {
        // Awoid context switch if already fired
        if (queue_head_.load() == kFired) {
            return;
        }
        DoAwait([this](IAwaiter* current_awaiter) {
            IAwaiter* expected_head = queue_head_.load();
            do {
                if (expected_head == kFired) {
                    return false;
                }
                current_awaiter->Next = expected_head;
            } while (!queue_head_.compare_exchange_strong(expected_head, current_awaiter));
            return true;
        });
    }

    void Event::Fire() {
        IAwaiter* head = queue_head_.exchange(kFired);
        while (head != kNotFired) {
            IAwaiter* next = head->Next;
            head->Wake();
            head = next;
        }
    }

}  // namespace NDummyConcurrency::NSynchronization::FiberAware