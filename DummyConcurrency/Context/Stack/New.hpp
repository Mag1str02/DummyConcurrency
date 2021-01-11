#pragma once

#include <DummyConcurrency/Context/StackView.hpp>

#include <cstdlib>
// std::exchange, std::swap
#include <utility>

namespace DummyConcurrency::Context {

    // NewStack = heap allocation view new char[size]

    class NewStack {
    private:
        struct Alloc {
            std::byte* start;
            size_t     size;
        };

    public:
        NewStack() = delete;

        // Non-copyable
        NewStack(const NewStack&)            = delete;
        NewStack& operator=(const NewStack&) = delete;

        // Move-constructible
        NewStack(NewStack&& that) : alloc_(std::exchange(that.alloc_, {nullptr, 0})) {}

        // Move-assignable
        NewStack& operator=(NewStack that) {
            Swap(that);
            return *this;
        }

        static NewStack AllocateBytes(size_t at_least) {
            size_t size = at_least;
            return NewStack{
                Alloc{new std::byte[size], size}
            };
        }

        void* LowerBound() const { return alloc_.start; }

        StackView MutView() { return {alloc_.start, alloc_.size}; }

        ~NewStack() {
            if (alloc_.start != nullptr) {
                delete[] alloc_.start;
            }
        }

    private:
        NewStack(Alloc alloc) : alloc_(alloc) {}

        void Swap(NewStack& that) { std::swap(alloc_, that.alloc_); }

    private:
        Alloc alloc_;
    };

}  // namespace DummyConcurrency::Context
