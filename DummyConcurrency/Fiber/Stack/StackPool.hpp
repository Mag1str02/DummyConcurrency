#pragma once

#include "IStackPool.hpp"
#include "New.hpp"
#include "Size.hpp"

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Synchronization/Thread/SpinLock.hpp>

namespace DummyConcurrency::Fiber {

    class StackPool : public IStackPool, public NonCopyable {
    public:
        explicit StackPool(StackSize size);

        virtual NewStack AllocateStack();
        virtual void     FreeStack(NewStack&& stack);
        void             Clear();
        uint64_t         Size() const;

    private:
        struct NewNode {
            NewStack Stack;
            NewNode* Next = nullptr;

            explicit NewNode(NewStack&& stack);
        };

    private:
        const uint64_t stack_size_;

        Synchronization::Thread::SpinLock     lock_;
        NewNode*                              stack_of_stack_ = nullptr;  // Hell yeah!
        ImplementationLayer::Atomic<uint64_t> size_           = 0;
    };

}  // namespace DummyConcurrency::Fiber
