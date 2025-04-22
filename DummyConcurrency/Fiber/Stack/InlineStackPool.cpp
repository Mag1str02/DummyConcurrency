#include "InlineStackPool.hpp"

#include "Size.hpp"

namespace DummyConcurrency::Fiber {

    class InlineStackPool : public IStackPool {
    public:
        virtual NewStack AllocateStack() { return NewStack::Allocate(SizeInBytes(StackSize::Medium)); }
        virtual void     FreeStack(NewStack&& stack) { NewStack dump_stack(std::move(stack)); }
    };

    IStackPool* InlineStackPool() {
        static class InlineStackPool sPool;
        return &sPool;
    }

}  // namespace DummyConcurrency::Fiber
