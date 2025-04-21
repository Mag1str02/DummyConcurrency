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
