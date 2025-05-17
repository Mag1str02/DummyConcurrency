#include "FiberInvoker.hpp"

#include <DummyConcurrency/Fiber/Core/Fiber.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace NDummyConcurrency::NRuntime {

    STATIC_THREAD_LOCAL_PTR(NFiber::Fiber, gCurrentFiberInvoker);

    namespace {
        void FiberBody(ITaskProvider* task_provider) {
            DC_PROFILE_SCOPE();
            while (true) {
                auto* task = task_provider->GetNextTask();
                if (task == nullptr) {
                    gCurrentFiberInvoker = nullptr;
                    return;
                }
                {
                    DC_PROFILE_NAMED_SCOPE("Task Run");
                    task->Run();
                }
                if (gCurrentFiberInvoker != NFiber::Fiber::Self()) {
                    return;
                }
            }
        }
    }  // namespace

    FiberInvoker::FiberInvoker(IFiberHintProvider* hint_provider, IScheduler* scheduler) : hint_provider_(hint_provider), scheduler_(scheduler) {}

    void FiberInvoker::Invoke(ITaskProvider* task_provider) noexcept {
        DC_PROFILE_SCOPE();
        do {
            Iteration(task_provider);
        } while (gCurrentFiberInvoker != nullptr);
    }

    void FiberInvoker::Iteration(ITaskProvider* task_provider) {
        DC_PROFILE_SCOPE();
        auto           hint  = hint_provider_->GetHint();
        NFiber::Fiber* fiber = NFiber::Fiber::Create(*scheduler_, [task_provider, hint, hinter = hint_provider_]() {
            FiberBody(task_provider);
            hinter->ReturnHint(hint);
        }, hint);
        gCurrentFiberInvoker = fiber;
        fiber->Run();
    }

}  // namespace NDummyConcurrency::NRuntime
