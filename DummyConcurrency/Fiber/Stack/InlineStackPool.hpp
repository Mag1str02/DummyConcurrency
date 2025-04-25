#pragma once

#include "Size.hpp"
#include "StackProvider.hpp"

namespace NDummyConcurrency::NFiber {

    template <uint64_t Size = SizeInBytes(StackSize::Medium)>
    IStackProvider* InlineStackPool() {
        class InlineStackPool : public IStackProvider {
        public:
            virtual NewStack AllocateStack() { return NewStack::Allocate(Size); }
            virtual void     FreeStack(NewStack&& stack) { NewStack dump_stack(std::move(stack)); }
        };

        static InlineStackPool sPool;
        return &sPool;
    }

}  // namespace NDummyConcurrency::NFiber
