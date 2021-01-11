#pragma once

#include <twist/assist/assert.hpp>

#include <atomic>

namespace Testing {

    template <typename T> struct Counted {
        static inline std::atomic_size_t gCount = 0;

        Counted() { IncrementCount(); }

        Counted(const Counted&) { IncrementCount(); }

        Counted(Counted&&) { IncrementCount(); }

        Counted& operator=(const Counted&) {
            // No-op
        }

        Counted& operator=(Counted&&) {
            // No-op
        }

        ~Counted() { DecrementCount(); }

        static size_t ObjectCount() { return gCount.load(); }

    private:
        static void IncrementCount() { gCount.fetch_add(1, std::memory_order::relaxed); }

        static void DecrementCount() { gCount.fetch_sub(1, std::memory_order::relaxed); }
    };

}  // namespace Testing
