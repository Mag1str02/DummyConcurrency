#pragma once

#include "AtomicSharedPtr.hpp"

#include <optional>

// Treiber unbounded lock-free stack
// https://en.wikipedia.org/wiki/Treiber_stack

namespace NDummyConcurrency::NDataStructures {

    template <typename T>
    class LockFreeStack {
        struct Node {
            explicit Node(T v) : Value(std::move(v)) {}

            T               Value;
            SharedPtr<Node> Next;
        };

    public:
        void Push(T value) {
            auto new_node  = MakeShared<Node>(std::move(value));
            new_node->Next = top_.Load();
            while (!top_.CompareExchangeWeak(new_node->Next, new_node)) {
                ;
            }
        }

        std::optional<T> TryPop() {
            while (true) {
                SharedPtr<Node> curr_top = top_.Load();
                if (!curr_top) {
                    return std::nullopt;
                }
                if (top_.CompareExchangeWeak(curr_top, curr_top->Next)) {
                    return std::move(curr_top->Value);
                }
            }
        }

    private:
        AtomicSharedPtr<Node> top_;
    };

}  // namespace NDummyConcurrency::NDataStructures
