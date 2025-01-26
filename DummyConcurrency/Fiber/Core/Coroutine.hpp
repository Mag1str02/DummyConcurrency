#pragma once

#include "Stack.hpp"

#include <DummyConcurrency/Context/Context.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Fiber {

    class ICoroutine : protected Context::ITrampoline {
    public:
        virtual ~ICoroutine() = default;

        ICoroutine(const ICoroutine& other)            = delete;
        ICoroutine& operator=(const ICoroutine& other) = delete;

        void Resume();
        void SwitchTo(ICoroutine& other);
        void Suspend();
        bool IsCompleted() const;

    protected:
        explicit ICoroutine(size_t stack_size);

        bool  completed_ = false;
        Stack stack_;

        Context::ExecutionContext  coro_context_;
        Context::ExecutionContext* caller_context_ = nullptr;

        ImplementationLayer::Fiber        impl_fiber_;
        ImplementationLayer::FiberHandle* impl_caller_fiber_ = nullptr;
    };  // namespace DummyConcurrency::Fiber

    template <typename F> class Coroutine : public ICoroutine {
    public:
        explicit Coroutine(F&& body, size_t stack_size) : ICoroutine(stack_size), body_(std::move(body)) {}

    private:
        virtual void Run() noexcept override {
            try {
                body_();
            } catch (...) {
                DC_PANIC("Exceptions are forbidden in coroutins");
            }
            ImplementationLayer::SwitchToFiber(*impl_caller_fiber_);

            completed_ = true;
            coro_context_.ExitTo(*caller_context_);
            std::unreachable();
        }

    private:
        F body_;
    };

}  // namespace DummyConcurrency::Fiber