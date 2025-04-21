#pragma once

namespace DummyConcurrency::Synchronization {

    class IAwaiter {
    public:
        virtual void Wake() noexcept = 0;
        virtual void Wait() noexcept = 0;

    public:
        IAwaiter* Next;
    };

}  // namespace DummyConcurrency::Synchronization
