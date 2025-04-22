#include "Size.hpp"

#include <DummyConcurrency/Utils/Assert.hpp>

namespace DummyConcurrency::Fiber {

    constexpr uint64_t operator""_GB(unsigned long long x) {
        return 1024ULL * 1024ULL * 1024ULL * x;
    }
    constexpr uint64_t operator""_MB(unsigned long long x) {
        return 1024ULL * 1024ULL * x;
    }
    constexpr uint64_t operator""_KB(unsigned long long x) {
        return 1024ULL * x;
    }

    uint64_t SizeInBytes(StackSize size) {
        switch (size) {
            case StackSize::Tiny: return 64_KB;
            case StackSize::Small: return 256_KB;
            case StackSize::Medium: return 1_MB;
            case StackSize::Large: return 8_MB;
            default: DC_PANIC("Invalid size");
        }
    }

}  // namespace DummyConcurrency::Fiber
