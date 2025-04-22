#pragma once

#include <DummyConcurrency/Context/StackView.hpp>

namespace NDummyConcurrency::NContext {

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

}  // namespace NDummyConcurrency::NContext
