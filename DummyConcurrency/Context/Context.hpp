#pragma once

#include <DummyConcurrency/Context/StackView.hpp>
#include <DummyConcurrency/Context/Trampoline.hpp>

#include <DummyConcurrency/Context/Exceptions/Context.hpp>
#include <DummyConcurrency/Context/Machine/Context.hpp>
#include <DummyConcurrency/Context/Sanitizer/Context.hpp>

namespace DummyConcurrency::Context {

    // Execution Context =
    // 1) Machine context (registers) +
    // 2) [Address | Thread] sanitizer context +
    // 3) Exceptions context

    class ExecutionContext final : private ITrampoline {
    public:
        // Empty context, cannot be a target for SwitchTo
        ExecutionContext();

        // Non-copyable
        ExecutionContext(const ExecutionContext&)            = delete;
        ExecutionContext& operator=(const ExecutionContext&) = delete;

        // Non-movable
        ExecutionContext(ExecutionContext&&)            = delete;
        ExecutionContext& operator=(ExecutionContext&&) = delete;

        // Prepare execution context for running trampoline->Run()
        // on stack `stack`
        void Setup(StackView stack, ITrampoline* trampoline);

        // Symmetric Control Transfer
        // 1) Save current execution context to `this`
        // 2) Activate `target` context
        void SwitchTo(ExecutionContext& target);

        // Leave current execution context forever
        // Last context switch in ITrampoline::Run
        [[noreturn]] void ExitTo(ExecutionContext& target);

        // For overflow checking
        // Calling StackPointer on non-suspended ExecutionContext results in UB
        void* StackPointer() const noexcept { return machine_.StackPointer(); }

    private:
        // ITrampoline
        void Run() noexcept override;

        // Finalize first context switch
        void AfterStart();

    private:
        ITrampoline* user_trampoline_;

        MachineContext    machine_;
        SanitizerContext  sanitizer_;
        ExceptionsContext exceptions_;
    };

}  // namespace DummyConcurrency::Context
