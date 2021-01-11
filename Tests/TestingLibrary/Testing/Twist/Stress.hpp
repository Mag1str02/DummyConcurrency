#pragma once

#include "Body.hpp"

#include <chrono>

namespace Testing::Twist {

    namespace Stress {

        struct Params {
            std::chrono::milliseconds TimeBudget;
        };

        void Test(TestBody body, Params params);

    }  // namespace Stress

}  // namespace Testing::Twist
