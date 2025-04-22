#pragma once

#include "New.hpp"

namespace DummyConcurrency::Fiber {

    class IStackPool;

    class LeasedStack : NonCopyable {
    public:
        NewStack* operator->();
        LeasedStack(LeasedStack&& other);
        LeasedStack& operator=(LeasedStack&& other) = delete;
        ~LeasedStack();

    private:
        friend class IStackPool;
        explicit LeasedStack(NewStack&& stack, IStackPool* pool);

    private:
        NewStack    stack_;
        IStackPool* pool_;
    };

    class IStackPool {
    public:
        LeasedStack GetStack();

    protected:
        virtual NewStack AllocateStack()             = 0;
        virtual void     FreeStack(NewStack&& stack) = 0;

    private:
        friend class LeasedStack;
    };

}  // namespace DummyConcurrency::Fiber
