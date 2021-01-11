#include "Result.hpp"

#include <fmt/core.h>

namespace Testing::Twist {
    namespace Simulation {

        std::string FormatFailure(::twist::sim::Result result) {
            return fmt::format("Simulation failure ({}): {}", result.status, result.std_err);
        }

    }  // namespace Simulation
}  // namespace Testing::Twist
