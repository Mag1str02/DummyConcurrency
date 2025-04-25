#include "Inline.hpp"

namespace NDummyConcurrency::NRuntime {

    class InlineInvoker : public IInvoker {
    public:
        virtual void Invoke(ITaskProvider* provider) noexcept override {
            auto* task = provider->GetNextTask();
            while (task != nullptr) {
                task->Run();
                task = provider->GetNextTask();
            }
        }
    };

    IInvoker& InlineInvoker() {
        static class InlineInvoker sInvoker;
        return sInvoker;
    }

}  // namespace NDummyConcurrency::NRuntime
