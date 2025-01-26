#pragma once

#include <thread>

namespace DummyConcurrency::ImplementationLayer::STD {

    class SpinWait {
    private:
        static constexpr size_t kSpinLimit  = 6;
        static constexpr size_t kYieldLimit = 10;

    public:
        SpinWait() = default;

        // Non-copyable
        SpinWait(const SpinWait&)            = delete;
        SpinWait& operator=(const SpinWait&) = delete;

        // Non-movable
        SpinWait(SpinWait&&)            = delete;
        SpinWait& operator=(SpinWait&&) = delete;

        void Spin() {
            if (count_ < kSpinLimit) {
                for (size_t j = 0; j < ((size_t)1 << count_); ++j) {
                    // CpuRelax
                }
            } else {
                std::this_thread::yield();
            }

            ++count_;
        }

        void operator()() { Spin(); }

        bool ConsiderParking() const { return count_ > kYieldLimit; }
        bool KeepSpinning() const { return !ConsiderParking(); }

    private:
        size_t count_ = 0;
    };

}  // namespace DummyConcurrency::ImplementationLayer::STD