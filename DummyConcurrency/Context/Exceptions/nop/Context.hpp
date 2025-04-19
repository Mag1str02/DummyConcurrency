#pragma once

#include <DummyConcurrency/Common.hpp>

#include <exception>

namespace DummyConcurrency::Context {

    struct ExceptionsContext {
        void SwitchTo(ExceptionsContext& /*target*/) {
            DC_ASSERT(std::uncaught_exceptions() == 0,
                      "Program was build with nop exception context, but is required to switch context while there is an exception in flight");
        }
    };

}  // namespace DummyConcurrency::Context
