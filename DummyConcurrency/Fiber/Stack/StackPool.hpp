#pragma once

#include "New.hpp"
#include "Size.hpp"
#include "StackProvider.hpp"

#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>
#include <DummyConcurrency/Synchronization/Thread/SpinLock.hpp>

namespace NDummyConcurrency::NFiber {

    class StackPool : public IStackProvider, public NonCopyable {
    public:
        explicit StackPool(StackSize size, uint64_t preallocate_count = 0);
        ~StackPool();

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

        NSynchronization::NThread::SpinLock    lock_;
        NewNode*                               stack_of_stack_ = nullptr;  // Hell yeah!
        NImplementationLayer::Atomic<uint64_t> size_           = 0;
    };

}  // namespace NDummyConcurrency::NFiber
