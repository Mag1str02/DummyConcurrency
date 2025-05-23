#pragma once

#include <DummyConcurrency/Utils/Assert.hpp>

#include <exception>

namespace NDummyConcurrency::NContext {

    struct ExceptionsContext {
        void SwitchTo(ExceptionsContext& /*target*/) {
            DC_ASSERT(std::uncaught_exceptions() == 0,
                      "Program was build with nop exception context, but is required to switch context while there is an exception in flight");
        }

        static bool IsSwitchAllowed() { return std::uncaught_exceptions() == 0; }
    };

}  // namespace NDummyConcurrency::NContext
