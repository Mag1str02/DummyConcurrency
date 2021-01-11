#pragma once

#include <wheels/core/stop_watch.hpp>
#include <wheels/test/current.hpp>

#include <algorithm>
#include <chrono>

namespace Testing {

    // Not thread-safe!

    class TimeBudget {
        using Clock = std::chrono::steady_clock;
        using Units = Clock::duration;

        static constexpr Units kBatchThreshold = std::chrono::milliseconds(10);

    public:
        explicit TimeBudget(Units budget) : budget_(budget) { start_ = Clock::now(); }

        TimeBudget() : TimeBudget(::wheels::test::TestTimeLeft()) {}

        bool KeepRunning() {
            static const Units kSafeMargin = std::chrono::milliseconds(250);

            ++count_;
            if (count_ == batch_) {
                Adapt();
                return TimeLeft() > kSafeMargin;
            } else {
                return true;
            }
        }

        operator bool() { return KeepRunning(); }  // NOLINT

    private:
        void Adapt() {
            auto elapsed = batch_timer_.Elapsed();

            if (elapsed * 2 < kBatchThreshold) {
                batch_ *= 2;
            } else {
                // Restart batch
                batch_timer_.Restart();
                count_ = 0;

                if (elapsed > kBatchThreshold * 2) {
                    batch_ = std::max(batch_ / 2, (size_t)1);
                }
            }
        }

        Units TimeLeft() const {
            auto now        = Clock::now();
            auto expiration = start_ + budget_;
            if (now > expiration) {
                return Units(0);
            } else {
                // expiration >= now
                return expiration - now;
            }
        }

    private:
        const Units       budget_;  // NOLINT
        Clock::time_point start_;

        ::wheels::StopWatch<> batch_timer_;
        size_t                count_ = 0;
        size_t                batch_ = 1;
    };

}  // namespace Testing
