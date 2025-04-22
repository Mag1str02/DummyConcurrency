#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <cstdint>

namespace DummyConcurrency::Fiber {

    // NewStack = heap allocation view new char[size]

    class NewStack : public NonCopyable {
    public:
        static NewStack Allocate(uint64_t at_least);

        NewStack() = delete;
        NewStack(NewStack&& that);
        NewStack& operator=(NewStack&& that);
        ~NewStack();

        void* Bottom() const;
        void* Top() const;

        Context::StackView View();
        void               Reset();

        template <typename T, typename... Args>
        T* Allocate(Args&&... args) {
            current_size_ -= sizeof(T);
            current_size_ &= ~(alignof(T) - 1);
            return new (start_ + current_size_) T(std::forward<Args>(args)...);
        }
        template <typename T>
        void* PreAllocate() {
            current_size_ -= sizeof(T);
            current_size_ &= ~(alignof(T) - 1);
            return start_ + current_size_;
        }

    private:
        explicit NewStack(std::byte* start, uint64_t size);
        void Swap(NewStack& that);

    private:
        std::byte* start_;
        uint64_t   size_;
        uint64_t   current_size_;
    };

}  // namespace DummyConcurrency::Fiber
