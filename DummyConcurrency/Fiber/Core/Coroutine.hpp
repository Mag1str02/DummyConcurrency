#pragma once

#include <DummyConcurrency/Context/Context.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace NDummyConcurrency::NFiber {

    class ICoroutine : protected NContext::ITrampoline {
    public:
        explicit ICoroutine(NContext::StackView view);
        virtual ~ICoroutine() = default;

        ICoroutine(const ICoroutine& other)            = delete;
        ICoroutine& operator=(const ICoroutine& other) = delete;

        virtual void Body() = 0;
        virtual void Run() noexcept override;

        void Resume();
        void SwitchTo(ICoroutine& other);
        void Suspend();
        bool IsCompleted() const;

        NImplementationLayer::Fiber* GetFiber();

    private:
        bool completed_ = false;

        NContext::ExecutionContext  coro_context_;
        NContext::ExecutionContext* caller_context_ = nullptr;

        NImplementationLayer::Fiber        impl_fiber_;
        NImplementationLayer::FiberHandle* impl_caller_fiber_ = nullptr;
    };  // namespace NDummyConcurrency::NFiber

}  // namespace NDummyConcurrency::NFiber