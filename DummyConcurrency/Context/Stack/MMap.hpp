#pragma once

#include <DummyConcurrency/Context/Stack/View.hpp>
#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

namespace DummyConcurrency::Context {

    class MmapAllocation : public NonCopyable {
    public:
        MmapAllocation() { Reset(); }

        static size_t PageSize();

        // Allocate `count` pages of zeroed memory
        static MmapAllocation AllocatePages(size_t count, void* hint = nullptr);
        static MmapAllocation Acquire(MutableMemView view);

        // Moving
        MmapAllocation(MmapAllocation&& that);
        MmapAllocation& operator=(MmapAllocation&& that);

        ~MmapAllocation() { Deallocate(); }

        char* Start() const { return start_; }

        char* End() const { return start_ + size_; }

        size_t Size() const { return size_; }

        size_t PageCount() const { return Size() / PageSize(); }

        ConstMemView View() const { return {start_, size_}; }

        MutableMemView MutView() { return {start_, size_}; }

        // Protect range of pages
        // Protected pages cannot be read, written or executed
        void ProtectPages(size_t start_index, size_t count);

        void Deallocate();

        MutableMemView Release();

    private:
        MmapAllocation(char* start, size_t size) : start_(start), size_(size) {}

        void Reset();

    private:
        // Aligned to page boundary
        char*  start_;
        size_t size_;
    };

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
        explicit MmapStack(MmapAllocation allocation);

        static MmapStack AllocatePages(size_t count);

    private:
        MmapAllocation allocation_;
    };

}  // namespace DummyConcurrency::Context
