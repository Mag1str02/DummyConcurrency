#include "New.hpp"

namespace DummyConcurrency::Fiber {

    NewStack::NewStack(NewStack&& that) {
        start_        = std::exchange(that.start_, nullptr);
        size_         = std::exchange(that.size_, 0);
        current_size_ = std::exchange(that.current_size_, 0);
    }

    // Move-assignable
    NewStack& NewStack::operator=(NewStack&& that) {
        Swap(that);
        return *this;
    }

    NewStack NewStack::Allocate(size_t at_least) {
        size_t size = at_least;
        return NewStack{new std::byte[size], size};
    }

    void NewStack::Reset() {
        current_size_ = size_;
    }
    void* NewStack::Bottom() const {
        return start_;
    }
    void* NewStack::Top() const {
        return start_ + current_size_;
    }

    Context::StackView NewStack::View() {
        return {start_, current_size_};
    }

    NewStack::~NewStack() {
        if (start_ != nullptr) {
            delete[] start_;
        }
    }

    NewStack::NewStack(std::byte* start, uint64_t size) : start_(start), size_(size), current_size_(size) {}

    void NewStack::Swap(NewStack& that) {
        std::exchange(that.start_, start_);
        std::exchange(that.size_, size_);
        std::exchange(that.current_size_, current_size_);
    }

}  // namespace DummyConcurrency::Fiber
