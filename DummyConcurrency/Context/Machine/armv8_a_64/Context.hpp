#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Context/Trampoline.hpp>

extern "C" {
#include "Context.h"
}

namespace NDummyConcurrency::NContext {

    // Target architecture: armv8-a

    struct MachineContext {
        void* rsp_;

        void Setup(StackView stack, ITrampoline* trampoline);

        void SwitchTo(MachineContext& target);

        static constexpr bool kStackPointerAvailable = true;

        void* StackPointer() const noexcept { return rsp_; }
    };

}  // namespace NDummyConcurrency::NContext
