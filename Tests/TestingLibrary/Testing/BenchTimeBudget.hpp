#pragma once

#include <chrono>

using namespace std::chrono_literals;

namespace Testing {

    // Not thread-safe!

    class BenchTimeBudget {
        using Clock = std::chrono::steady_clock;
        using Units = Clock::duration;

    public:
        explicit BenchTimeBudget(Units budget) : budget_(budget) { start_ = Clock::now(); }

        bool KeepRunning() { return start_ + budget_ > Clock::now(); }

        operator bool() { return KeepRunning(); }  // NOLINT

    private:
        const Units       budget_;
        Clock::time_point start_;
    };

}  // namespace Testing
