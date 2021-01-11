#pragma once

#include "Testing/Twist/Model.hpp"  // IWYU pragma: export
#include "Twist/Randomize.hpp"      // IWYU pragma: export
#include "Twist/Stress.hpp"         // IWYU pragma: export

#include <wheels/test/framework.hpp>

#include <chrono>  // IWYU pragma: export

////////////////////////////////////////////////////////////////////////////////

// Stress testing

#define TWIST_STRESS_TEST(name, time_budget)                                                \
    void TwistStressTestBody##name();                                                       \
    TEST(name, ::wheels::test::TestOptions().TimeLimit(time_budget)) {                      \
        ::Testing::Twist::Stress::Test([] { TwistStressTestBody##name(); }, {time_budget}); \
    }                                                                                       \
    void TwistStressTestBody##name()

////////////////////////////////////////////////////////////////////////////////

// Stateless model checking

#define TWIST_MODEL(name, params)                                                                \
    void TwistModelBody##name();                                                                 \
    TEST(name, ::wheels::test::TestOptions().TimeLimit(::Testing::Twist::Model::TimeBudget())) { \
        static_assert(::Testing::Twist::Model::BuildSupported());                                \
        ::Testing::Twist::Model::Check([] { TwistModelBody##name(); }, params);                  \
    }                                                                                            \
    void TwistModelBody##name()

////////////////////////////////////////////////////////////////////////////////

// Randomized checking

#define TWIST_RANDOMIZE(name, time_budget)                                                                         \
    void TwistRandomCheckBody##name();                                                                             \
    TEST(name, ::wheels::test::TestOptions().TimeLimit(::Testing::Twist::Randomize::TestTimeLimit(time_budget))) { \
        static_assert(::Testing::Twist::Randomize::BuildSupported());                                              \
        ::Testing::Twist::Randomize::Check([] { TwistRandomCheckBody##name(); }, {time_budget});                   \
    }                                                                                                              \
    void TwistRandomCheckBody##name()
