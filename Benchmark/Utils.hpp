#pragma once

#include <chrono>

using Clock     = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

class Timer {
public:
    Timer() : start_(Clock::now()) {}

    double GetDuration() { return std::chrono::duration<double>(Clock::now() - start_).count(); }

private:
    TimePoint start_;
};

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
