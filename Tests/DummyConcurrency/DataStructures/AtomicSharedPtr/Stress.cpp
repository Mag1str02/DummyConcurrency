#include <DummyConcurrency/DummyConcurrency.hpp>

#include <twist/ed/std/thread.hpp>

#include <Testing/TimeBudget.hpp>
#include <Testing/Twist.hpp>

using namespace NDummyConcurrency;  // NOLINT

TEST_SUITE(StressAtomicSharedPtr) {
    void Race() {
        AtomicSharedPtr<std::string> asp;

        asp.Store(MakeShared<std::string>("Initial"));

        twist::ed::std::thread reader([&] {
            auto sp = asp.Load();
            ASSERT_TRUE(sp);
        });

        twist::ed::std::thread writer([&] { asp.Store(MakeShared<std::string>("Rewritten")); });

        reader.join();
        writer.join();
    }

    TWIST_STRESS_TEST(StoreLoad, 10s) {
        Testing::TimeBudget budget;

        while (budget) {
            Race();
        }
    }
}
