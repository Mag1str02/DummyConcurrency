#pragma once

#include <DummyConcurrency/Context/Context.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace DummyConcurrency::Fiber {

    class ICoroutine : protected Context::ITrampoline {
    public:
        explicit ICoroutine(Context::StackView view);
        virtual ~ICoroutine() = default;

        ICoroutine(const ICoroutine& other)            = delete;
        ICoroutine& operator=(const ICoroutine& other) = delete;

        virtual void Body() = 0;
        virtual void Run() noexcept override;

        void Resume();
        void SwitchTo(ICoroutine& other);
        void Suspend();
        bool IsCompleted() const;

    private:
        bool completed_ = false;

        Context::ExecutionContext  coro_context_;
        Context::ExecutionContext* caller_context_ = nullptr;

        ImplementationLayer::Fiber        impl_fiber_;
        ImplementationLayer::FiberHandle* impl_caller_fiber_ = nullptr;
    };  // namespace DummyConcurrency::Fiber

}  // namespace DummyConcurrency::Fiber