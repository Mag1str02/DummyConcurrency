#pragma once

#include <DummyConcurrency/Context/StackView.hpp>

#include <sanitizer/tsan_interface.h>

namespace DummyConcurrency::Context {

    struct SanitizerContext {
        void Setup(StackView /*stack*/) { fiber_ = __tsan_create_fiber(0); }

        void AfterStart() { After(); }

        // NB: __tsan_switch_to_fiber should be called immediately before switch to fiber
        // https://github.com/llvm/llvm-project/blob/712dfec1781db8aa92782b98cac5517db548b7f9/compiler-rt/include/sanitizer/tsan_interface.h#L150-L151
        __attribute__((always_inline)) inline void BeforeSwitch(SanitizerContext& target) {
            fiber_ = __tsan_get_current_fiber();
            __tsan_switch_to_fiber(target.fiber_, 0);
        }

        void AfterSwitch() { After(); }

        // NB: __tsan_switch_to_fiber should be called immediately before switch to fiber
        __attribute__((always_inline)) inline void BeforeExit(SanitizerContext& target) {
            target.exit_from_ = this;
            __tsan_switch_to_fiber(target.fiber_, 0);
        }

    private:
        void After() {
            if (exit_from_ != nullptr) {
                __tsan_destroy_fiber(exit_from_->fiber_);
                exit_from_ = nullptr;
            }
        }

    private:
        void*             fiber_;
        SanitizerContext* exit_from_{nullptr};
    };

}  // namespace DummyConcurrency::Context
