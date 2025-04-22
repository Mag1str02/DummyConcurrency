#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Context/Trampoline.hpp>

#include <ucontext.h>

namespace NDummyConcurrency::NContext {

    struct MachineContext {
        ucontext_t context;

        MachineContext();

        void Setup(StackView stack, ITrampoline* trampoline);
        void SwitchTo(MachineContext& target);

        static constexpr bool kStackPointerAvailable = true;

        void* StackPointer() const noexcept;
    };

}  // namespace NDummyConcurrency::NContext
