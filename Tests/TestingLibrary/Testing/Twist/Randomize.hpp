#pragma once

#include "Body.hpp"

#include <twist/build.hpp>

#include <chrono>

namespace Testing::Twist {

    namespace Randomize {

        constexpr bool BuildSupported() {
            return ::twist::build::Faulty() || ::twist::build::Sim();
        }

        std::chrono::milliseconds TestTimeLimit(std::chrono::milliseconds budget);

        struct Params {
            std::chrono::milliseconds TimeBudget;
        };

        void Check(TestBody body, Params params);

    }  // namespace Randomize

}  // namespace Testing::Twist
