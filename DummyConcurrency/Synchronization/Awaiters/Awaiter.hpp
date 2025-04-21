#pragma once

#include <DummyConcurrency/Utils/IntrusiveForwardList.hpp>

namespace DummyConcurrency::Synchronization {

    class IAwaiter : public IntrusiveForwardListNode<IAwaiter> {
    public:
        virtual void Wake() noexcept = 0;
    };

}  // namespace DummyConcurrency::Synchronization
