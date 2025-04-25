#include "FiberInvoker.hpp"

#include <DummyConcurrency/Fiber/Core/Fiber.hpp>
#include <DummyConcurrency/ImplementationLayer/ImplementationLayer.hpp>

namespace NDummyConcurrency::NRuntime {

    STATIC_THREAD_LOCAL_PTR(NFiber::Fiber, gCurrentFiberInvoker);

    namespace {
        void FiberBody(ITaskProvider* task_provider) {
            while (true) {
                auto* task = task_provider->GetNextTask();
                if (task == nullptr) {
                    gCurrentFiberInvoker = nullptr;
                    return;
                }
                task->Run();
                if (gCurrentFiberInvoker != NFiber::Fiber::Self()) {
                    return;
                }
            }
        }
    }  // namespace

    FiberInvoker::FiberInvoker(NFiber::IStackProvider* stack_provider, IScheduler* scheduler) :
        stack_provider_(stack_provider), scheduler_(scheduler) {}

    void FiberInvoker::Invoke(ITaskProvider* task_provider) noexcept {
        do {
            NFiber::Fiber* fiber =
                NFiber::Fiber::Create(*scheduler_, [task_provider]() { FiberBody(task_provider); }, {.StackProvider = stack_provider_});
            gCurrentFiberInvoker = fiber;
            fiber->Run();
        } while (gCurrentFiberInvoker != nullptr);
    }

}  // namespace NDummyConcurrency::NRuntime
