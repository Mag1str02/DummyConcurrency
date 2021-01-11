#pragma once

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/futex.hpp>

namespace DummyConcurrency::Threading {

    namespace Futex = twist::ed::futex;

    class WaitGroup {
    public:
        void Add(size_t cnt);
        void Done();
        void Wait();

    private:
        static constexpr uint32_t kWaitFlag         = 0x80000000;
        static constexpr uint32_t kWaitFlagReversed = kWaitFlag ^ ~(0);

        twist::ed::std::atomic<uint32_t> job_cnt_{0};
        twist::ed::std::atomic<uint32_t> futex_state_{0};
    };

}  // namespace DummyConcurrency::Threading
