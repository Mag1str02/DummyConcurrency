#pragma once

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency {

    class WaitGroup {
    public:
        void Add(size_t cnt);
        void Done();
        void Wait();

    private:
        static constexpr uint32_t kWaitFlag         = 0x80000000;
        static constexpr uint32_t kWaitFlagReversed = kWaitFlag ^ ~(0);

        ImplementationLayer::Atomic<uint32_t> job_cnt_{0};
        ImplementationLayer::Atomic<uint32_t> futex_state_{0};
    };

}  // namespace DummyConcurrency::Threading
