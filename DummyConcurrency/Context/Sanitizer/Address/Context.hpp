#pragma once

#include <DummyConcurrency/Context/StackView.hpp>

#include <sanitizer/asan_interface.h>

namespace DummyConcurrency::Context {

    struct SanitizerContext {
        void Setup(StackView stack) {
            stack_      = stack.data();
            stack_size_ = stack.size();
        }

        void AfterStart() { __sanitizer_finish_switch_fiber(nullptr, &(from_->stack_), &(from_->stack_size_)); }

        void BeforeSwitch(SanitizerContext& target) {
            target.from_ = this;
            __sanitizer_start_switch_fiber(&fake_stack_, target.stack_, target.stack_size_);
        }

        void AfterSwitch() { __sanitizer_finish_switch_fiber(fake_stack_, &(from_->stack_), &(from_->stack_size_)); }

        void BeforeExit(SanitizerContext& target) {
            target.from_ = this;
            // https://github.com/llvm-mirror/compiler-rt/blob/69445f095c22aac2388f939bedebf224a6efcdaf/include/sanitizer/common_interface_defs.h#L299
            __sanitizer_start_switch_fiber(nullptr, target.stack_, target.stack_size_);
        }

    private:
        const void* stack_;
        size_t      stack_size_;
        void*       fake_stack_;

        SanitizerContext* from_;
    };

}  // namespace DummyConcurrency::Context
