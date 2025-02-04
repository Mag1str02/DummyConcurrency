#pragma once

#include <chrono>
#include <cstdlib>
#include <ctime>

namespace Testing {

    class ProcessCPUTimer {
    public:
        ProcessCPUTimer() { Reset(); }

        std::chrono::microseconds Spent() const { return std::chrono::microseconds(SpentMicros()); }

        // Backward compatibility
        std::chrono::microseconds Elapsed() const { return Spent(); }

        void Reset() { start_ts_ = std::clock(); }

    private:
        size_t SpentMicros() const {
#if WINDOWS
            const size_t clocks = 0;
#else
            const size_t clocks = std::clock() - start_ts_;
#endif
            return ClocksToMicros(clocks);
        }

        static size_t ClocksToMicros(const size_t clocks) { return (clocks * 1'000'000) / CLOCKS_PER_SEC; }

    private:
        std::clock_t start_ts_;
    };

    class ThreadCPUTimer {
    public:
        ThreadCPUTimer() { Reset(); }

        std::chrono::nanoseconds Spent() const { return std::chrono::nanoseconds(SpentNanos()); }

        // Backward compatibility
        std::chrono::nanoseconds Elapsed() const { return Spent(); }

        void Reset() { clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_); }

    private:
        uint64_t SpentNanos() const {
            struct timespec now;
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);

            return ToNanos(now) - ToNanos(start_);
        }

        static uint64_t ToNanos(const struct timespec& tp) { return tp.tv_sec * 1'000'000'000 + tp.tv_nsec; }

    private:
        struct timespec start_;
    };

}  // namespace Testing
