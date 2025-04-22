#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <cstdint>

namespace DummyConcurrency::Fiber {

    // NewStack = heap allocation view new char[size]

    class NewStack : public NonCopyable {
    public:
        static NewStack Allocate(uint64_t at_least) {
            size_t size = at_least;
            return NewStack{new std::byte[size], size};
        }

        NewStack()                           = delete;
        NewStack& operator=(NewStack&& that) = delete;
        NewStack(NewStack&& that) : start_(that.start_), size_(that.size_), current_size_(that.current_size_) {
            that.start_        = nullptr;
        }
        ~NewStack() {
            if (start_ != nullptr) {
                delete[] start_;
            }
        }

        void* Bottom() const { return start_; }
        void* Top() const { return start_ + current_size_; }

        Context::StackView View() { return {start_, start_ + current_size_}; }
        void               Reset() { current_size_ = size_; }

        template <typename T>
        void* PreAllocate() {
            current_size_ -= sizeof(T);
            current_size_ &= ~(alignof(T) - 1);
            return start_ + current_size_;
        }

    private:
        explicit NewStack(std::byte* start, uint64_t size) : start_(start), size_(size), current_size_(size) {}

    private:
        std::byte* start_;
        uint64_t   size_;
        uint64_t   current_size_;
    };

}  // namespace DummyConcurrency::Fiber
