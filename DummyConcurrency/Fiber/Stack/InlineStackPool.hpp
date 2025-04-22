#pragma once

#include "IStackPool.hpp"
#include "Size.hpp"

namespace DummyConcurrency::Fiber {

    template <uint64_t Size = SizeInBytes(StackSize::Medium)>
    IStackPool* InlineStackPool() {
        class InlineStackPool : public IStackPool {
        public:
            virtual NewStack AllocateStack() { return NewStack::Allocate(Size); }
            virtual void     FreeStack(NewStack&& stack) { NewStack dump_stack(std::move(stack)); }
        };

        static InlineStackPool sPool;
        return &sPool;
    }

}  // namespace DummyConcurrency::Fiber
