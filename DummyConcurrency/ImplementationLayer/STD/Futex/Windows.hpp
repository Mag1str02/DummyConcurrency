#pragma once

#include <synchapi.h>

#include <atomic>
#include <cstdint>

namespace NDummyConcurrency::NImplementationLayer::STD {

    struct WakeKey {
        std::atomic<uint32_t>* Location;
    };

    inline WakeKey PrepareWake(std::atomic<uint32_t>& atomic) {
        return WakeKey{.Location = &atomic};
    }
    inline void Wait(std::atomic<uint32_t>& loc, uint32_t old) {
        WaitOnAddress(&loc, &old, sizeof(loc), UINT32_MAX);
    }
    inline void WakeOne(WakeKey key) {
        WakeByAddressSingle(key.Location);
    }
    inline void WakeAll(WakeKey key) {
        WakeByAddressAll(key.Location);
    }

}  // namespace NDummyConcurrency::NImplementationLayer::STD