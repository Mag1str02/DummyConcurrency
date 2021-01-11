#pragma once

#include "View.hpp"

namespace tmp_wheels {

    struct NonCopyable {
        NonCopyable(const NonCopyable& that)            = delete;
        NonCopyable& operator=(const NonCopyable& that) = delete;

    protected:
        // To be used only as a base class
        NonCopyable()  = default;
        ~NonCopyable() = default;
    };

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

}  // namespace tmp_wheels