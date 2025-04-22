#include <DummyConcurrency/Context/Machine/x86_64_unix/Context.hpp>

namespace DummyConcurrency::Context {

    // https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/
    static void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void* arg7) {
        ITrampoline* t = (ITrampoline*)arg7;
        t->Run();
    }

    void MachineContext::Setup(StackView stack, ITrampoline* trampoline) {
        rsp_ = SetupMachineContext((void*)stack.Top, (void*)MachineContextTrampoline, (void*)trampoline);
    }

}  // namespace DummyConcurrency::Context
