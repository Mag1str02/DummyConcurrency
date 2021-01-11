#pragma once

#include <atomic>
#include <functional>

namespace Testing {

    // For producer/consumer tests

    template <typename T> class CheckSum {
    public:
        void Produce(const T& item) { Feed(item); }

        void Consume(const T& item) { Feed(item); }

        bool Validate() { return value_.load() == 0; }

        size_t Value() const { return value_.load(); }

    private:
        void Feed(const T& item) { value_.fetch_xor(std::hash<T>{}(item), std::memory_order::relaxed); }

    private:
        std::atomic<size_t> value_{0};
    };

}  // namespace Testing
