#include "WaitGroup.hpp"

namespace DummyConcurrency::Synchronization::Thread {

    void WaitGroup::Add(size_t cnt) {
        job_cnt_.fetch_add(cnt);
    }

    void WaitGroup::Done() {
        uint32_t old = job_cnt_.fetch_sub(1) - 1;
        if ((old & kWaitFlag) != 0 && (old & kWaitFlagReversed) == 0) {
            auto key = ImplementationLayer::Futex::PrepareWake(job_cnt_);
            job_cnt_.fetch_and(kWaitFlagReversed);
            ImplementationLayer::Futex::WakeAll(key);
        }
    }

    void WaitGroup::Wait() {
        uint32_t latest = job_cnt_.load();
        while (latest != 0) {
            if ((latest & kWaitFlag) != 0 || job_cnt_.compare_exchange_strong(latest, latest | kWaitFlag)) {
                ImplementationLayer::Futex::Wait(job_cnt_, latest | kWaitFlag);
            }
            latest = job_cnt_.load();
        }
    }

}  // namespace DummyConcurrency::Synchronization::Thread