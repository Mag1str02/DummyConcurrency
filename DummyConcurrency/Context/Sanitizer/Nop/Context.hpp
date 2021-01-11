#pragma once

#include <DummyConcurrency/Context/StackView.hpp>

namespace DummyConcurrency::Context {

    struct SanitizerContext {
        void Setup(StackView) {
            // Nop
        }

        void AfterStart() {
            // Nop
        }

        void BeforeSwitch(SanitizerContext& /*target*/) {
            // Nop
        }

        void AfterSwitch() {
            // Nop
        }

        void BeforeExit(SanitizerContext& /*target*/) {
            // Nop
        }
    };

}  // namespace DummyConcurrency::Context
