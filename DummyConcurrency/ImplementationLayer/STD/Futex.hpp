#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <atomic>
#include <cstdint>

namespace DummyConcurrency::ImplementationLayer::STD {

    struct WakeKey {
        std::atomic<uint32_t>* Location;
    };

    inline int __futex(unsigned int* uaddr, int op, int val, const struct timespec* timeout, int* uaddr2, int val3) {  // NOLINT
        return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
    }

    inline WakeKey PrepareWake(std::atomic<uint32_t>& atomic) {
        return WakeKey{.Location = &atomic};
    }
    inline int Wait(std::atomic<uint32_t>& loc, uint32_t old) {
        return __futex((uint32_t*)&loc, FUTEX_WAIT_PRIVATE, old, nullptr, nullptr, 0);
    }
    inline int WakeOne(WakeKey key) {
        return __futex((uint32_t*)key.Location, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
    }
    inline int WakeAll(WakeKey key) {
        return __futex((uint32_t*)key.Location, FUTEX_WAKE_PRIVATE, INT32_MAX, nullptr, nullptr, 0);
    }

    //     using twist::ed::futex::PrepareWake;
    //     using twist::ed::futex::Wait;
    //     using twist::ed::futex::WakeAll;
    //     using twist::ed::futex::WakeOne;
}  // namespace DummyConcurrency::ImplementationLayer::STD