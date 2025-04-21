#pragma once

#include "DummyConcurrency/Utils/IntrusiveForwardList.hpp"

namespace DummyConcurrency::Runtime {

    struct ITask : IntrusiveForwardListNode<ITask> {
        virtual ~ITask() = default;

        virtual void Run() noexcept = 0;
    };

}  // namespace DummyConcurrency::Runtime
