#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Utils/Traits.hpp>

#include <cstdlib>
#include <utility>

namespace DummyConcurrency::Context {

    // NewStack = heap allocation view new char[size]

    class NewStack : public NonCopyable {
    private:
        struct Alloc {
            std::byte* Start;
            size_t     Size;
        };

    public:
        NewStack() = delete;

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

        void* LowerBound() const { return alloc_.Start; }

        StackView MutView() { return {alloc_.Start, alloc_.Size}; }

        ~NewStack() {
            if (alloc_.Start != nullptr) {
                delete[] alloc_.Start;
            }
        }

    private:
        explicit NewStack(Alloc alloc) : alloc_(alloc) {}

        void Swap(NewStack& that) { std::swap(alloc_, that.alloc_); }

    private:
        Alloc alloc_;
    };

}  // namespace DummyConcurrency::Context
