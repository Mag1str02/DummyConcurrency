#pragma once

#include <DummyConcurrency/Context/StackView.hpp>

#include <DummyConcurrency/Context/Wheels/MMap.hpp>

#include <cstdint>

namespace DummyConcurrency::Context {

    // MmapStack = mmap allocation with guard page

    class MmapStack {
    public:
        MmapStack() = delete;

        static MmapStack AllocateBytes(size_t at_least);

        // Allocated memory will be released to the operating system
        ~MmapStack() = default;

        MmapStack(MmapStack&&)            = default;
        MmapStack& operator=(MmapStack&&) = default;

        void* LowerBound() const;

        // Including guard page
        size_t AllocationSize() const { return allocation_.Size(); }

        StackView MutView();

        // Release / acquire ownership for the underlying memory region
        [[nodiscard("Memory leak")]] StackView Release();
        static MmapStack                       Acquire(StackView view);

    private:
        MmapStack(tmp_wheels::MmapAllocation allocation);

        static MmapStack AllocatePages(size_t count);

    private:
        tmp_wheels::MmapAllocation allocation_;
    };

}  // namespace DummyConcurrency::Context
