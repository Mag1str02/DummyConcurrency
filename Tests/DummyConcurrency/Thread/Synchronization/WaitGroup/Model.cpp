#include <DummyConcurrency/DummyConcurrency.hpp>

#include <Testing/Twist.hpp>

#include <twist/assist/shared.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/test/assert.hpp>

using namespace DummyConcurrency;  // NOLINT

TEST_SUITE(ThreadSynchronizationWaitGroup_Model) {
    const auto kParams = Testing::Twist::Model::Params{.MaxPreemptions = 4};

    TWIST_MODEL(AddAddWait, kParams) {
        TWaitGroup wg;

        twist::assist::Shared<bool> f1{false};
        twist::assist::Shared<bool> f2{false};

        wg.Add(1);

        twist::ed::std::thread t1([&] {
            f1.Write(true);
            wg.Done();
        });

        wg.Add(1);

        twist::ed::std::thread t2([&] {
            f2.Write(true);
            wg.Done();
        });

        twist::ed::std::thread t3([&] {
            wg.Wait();

            bool r1 = f1.Read();
            bool r2 = f2.Read();

            TWIST_TEST_ASSERT(r1 && r2, "Unfinished work");
        });

        t1.join();
        t2.join();
        t3.join();
    }

    TWIST_MODEL(AddWaitAdd, kParams) {
        TWaitGroup wg;

        twist::assist::Shared<bool> f1{false};
        twist::assist::Shared<bool> f2{false};

        wg.Add(1);

        twist::ed::std::thread t2;

        twist::ed::std::thread t1([&] {
            wg.Add(1);

            t2 = twist::ed::std::thread([&] {
                f2.Write(true);
                wg.Done();
            });

            f1.Write(true);
            wg.Done();
        });

        twist::ed::std::thread t3([&] {
            wg.Wait();

            bool r1 = f1.Read();
            bool r2 = f2.Read();

            TWIST_TEST_ASSERT(r1 && r2, "Unfinished work");
        });

        t1.join();
        t2.join();
        t3.join();
    }
}
