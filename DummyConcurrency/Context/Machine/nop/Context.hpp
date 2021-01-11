#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Context/Trampoline.hpp>

namespace DummyConcurrency::Context {

    struct MachineContext {
        MachineContext();

        void Setup(StackView stack, ITrampoline* trampoline);
        void SwitchTo(MachineContext& target);

        static constexpr bool kStackPointerAvailable = false;

        void* StackPointer() const noexcept;
    };

}  // namespace DummyConcurrency::Context
