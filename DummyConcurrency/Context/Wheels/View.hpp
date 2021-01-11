#pragma once

#include <cstdlib>

#include <DummyConcurrency/Common.hpp>

namespace tmp_wheels {

    // View on a contiguous chunk of writeable / read-only memory
    // The memory is owned by some other object and that object must maintain the
    // memory as long as the span references it.

    //////////////////////////////////////////////////////////////////////

    class ConstMemView {
    public:
        ConstMemView(const char* start, size_t size) : start_(start), size_(size) {}

        ConstMemView() : ConstMemView(nullptr, 0) {}

        const char* Data() const noexcept { return start_; }

        size_t Size() const noexcept { return size_; }

        const char* Begin() const noexcept { return start_; }

        const char* End() const noexcept { return start_ + size_; }

        bool IsEmpty() const noexcept { return size_ == 0; }

        bool IsValid() const noexcept { return start_ != nullptr; }

        void operator+=(size_t offset) noexcept {
            DC_ASSERT(offset <= size_, "Out of bounds");
            start_ += offset;
            size_ -= offset;
        }

        std::string_view AsStringView() const noexcept { return {start_, size_}; }

        static ConstMemView Empty() { return {nullptr, 0}; }

    private:
        const char* start_;
        size_t      size_;
    };

    //////////////////////////////////////////////////////////////////////

    struct MutableMemView {
    public:
        MutableMemView(char* start, size_t size) : start_(start), size_(size) {}

        MutableMemView() : MutableMemView(nullptr, 0) {}

        char* Data() const noexcept { return start_; }

        size_t Size() const noexcept { return size_; }

        char* Begin() const noexcept { return start_; }

        char* End() const noexcept { return start_ + size_; }

        char* Back() const noexcept {
            DC_ASSERT(size_ > 0, "Empty");
            return End() - 1;
        }

        bool HasSpace() const noexcept { return size_ > 0; }

        bool IsValid() const noexcept { return start_ != nullptr; }

        void operator+=(size_t offset) noexcept {
            DC_ASSERT(offset <= size_, "Out of bounds");
            start_ += offset;
            size_ -= offset;
        }

        operator ConstMemView() const noexcept { return {start_, size_}; }

    private:
        char*  start_;
        size_t size_;
    };

}  // namespace tmp_wheels