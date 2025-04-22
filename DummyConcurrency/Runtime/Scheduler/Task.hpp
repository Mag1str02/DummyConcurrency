#pragma once

#include "DummyConcurrency/Utils/IntrusiveForwardList.hpp"

namespace NDummyConcurrency::NRuntime {

    struct ITask : IntrusiveForwardListNode<ITask> {
        virtual ~ITask() = default;

        virtual void Run() noexcept = 0;
    };

}  // namespace NDummyConcurrency::Runtime
