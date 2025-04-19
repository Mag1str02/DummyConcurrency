#include <DummyConcurrency/DummyConcurrency.hpp>

#include <twist/ed/std/thread.hpp>

#include <Testing/Twist.hpp>

#include <twist/assist/shared.hpp>
#include <twist/test/assert.hpp>
#include <twist/test/lock_free.hpp>

using namespace DummyConcurrency;  // NOLINT

static_assert(twist::build::IsolatedSim());

TEST_SUITE(AtomicSharedPtr) {
    TWIST_RANDOMIZE(Race, 5s) {
        using SharedInt = twist::assist::Shared<int>;

        // Init

        AtomicSharedPtr<SharedInt> asp;

        asp.Store(MakeShared<SharedInt>(1));

        // Concurrent

        twist::ed::std::thread reader([&] {
            twist::test::LockFreeScope lf;

            auto sp = asp.Load();

            twist::test::Progress();

            TWIST_TEST_ASSERT(sp, "Expected non-null pointer");

            int v = sp->Read();  // Happens-before checking

            TWIST_TEST_ASSERT(v == 1 || v == 2, "Unexpected value");
        });

        twist::ed::std::thread writer([&] {
            twist::test::LockFreeScope lf;

            asp.Store(MakeShared<SharedInt>(2));
            twist::test::Progress();
        });

        reader.join();
        writer.join();
    }
}
