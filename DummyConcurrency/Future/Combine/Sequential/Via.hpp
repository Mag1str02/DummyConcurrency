#pragma once

#include <DummyConcurrency/Runtime/Scheduler/Scheduler.hpp>

#include <DummyConcurrency/Future/Future.hpp>
#include <DummyConcurrency/Future/State/Contract.hpp>
#include <DummyConcurrency/Future/Syntax/Pipe.hpp>

#include <cstdlib>  // std::abort

namespace DummyConcurrency::Future {

    namespace Combinators {

        struct [[nodiscard]] Via {
            Runtime::IScheduler* Scheduler;

            explicit Via(Runtime::IScheduler& s) : Scheduler(&s) {}

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

    inline auto Via(Runtime::IScheduler& scheduler) {
        return Combinators::Via{scheduler};
    }

}  // namespace DummyConcurrency::Future
