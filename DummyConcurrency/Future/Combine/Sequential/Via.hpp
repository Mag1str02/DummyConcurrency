#pragma once

#include <DummyConcurrency/Scheduler/Interface/Scheduler.hpp>

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Syntax/Pipe.hpp>

#include <cstdlib>  // std::abort

namespace DummyConcurrency::Future {

    namespace Combinators {

        struct [[nodiscard]] Via {
            Scheduler::IScheduler* Scheduler;

            explicit Via(Scheduler::IScheduler& s) : Scheduler(&s) {}

            // Non-copyable
            Via(const Via&) = delete;

            template <typename T>
            Future<T> Pipe(Future<T> future) {
                future.SetScheduler(*Scheduler);
                return future;
            }
        };

    }  // namespace Combinators

    /*
     * Scheduling state
     *
     * Future<T> -> Scheduler -> Future<T>
     *
     */

    inline auto Via(Scheduler::IScheduler& scheduler) {
        return Combinators::Via{scheduler};
    }

}  // namespace DummyConcurrency::Future
