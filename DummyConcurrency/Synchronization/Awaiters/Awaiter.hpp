#pragma once

#include <DummyConcurrency/Utils/IntrusiveForwardList.hpp>

namespace NDummyConcurrency::NSynchronization {

    class IAwaiter : public IntrusiveForwardListNode<IAwaiter> {
    public:
        virtual void Wake() noexcept = 0;
    };

}  // namespace NDummyConcurrency::NSynchronization
