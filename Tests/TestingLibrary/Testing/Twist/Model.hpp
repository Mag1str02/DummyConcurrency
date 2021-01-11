#pragma once

#include "Body.hpp"

#include <twist/build.hpp>

#include <chrono>
#include <optional>

namespace Testing::Twist {

    namespace Model {

        constexpr bool BuildSupported() {
            return ::twist::build::IsolatedSim();
        }

        constexpr std::chrono::seconds TimeBudget() {
            return std::chrono::minutes(5);
        }

        struct Params {
            std::optional<size_t> MaxPreemptions   = std::nullopt;
            std::optional<size_t> MaxSteps         = std::nullopt;
            bool                  SpuriousWakeups  = false;
            bool                  SpuriousFailures = false;
        };

        void Check(TestBody body, Params params);

    }  // namespace Model

}  // namespace Testing::Twist
