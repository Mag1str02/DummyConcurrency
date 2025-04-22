#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Context/Trampoline.hpp>

extern "C" {
#include "Context.h"
}

namespace NDummyConcurrency::NContext {

    // Target architecture: x86-64

    struct MachineContext {
        void* rsp_;

        void Setup(StackView stack, ITrampoline* trampoline);

        void SwitchTo(MachineContext& target) { SwitchMachineContext(&rsp_, &target.rsp_); }

        static constexpr bool kStackPointerAvailable = true;

        void* StackPointer() const noexcept { return rsp_; }
    };

}  // namespace NDummyConcurrency::NContext
