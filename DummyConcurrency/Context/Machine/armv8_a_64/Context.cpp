#include <DummyConcurrency/Context/Machine/armv8_a_64/Context.hpp>

namespace DummyConcurrency::Context {

    static void MachineContextTrampoline(void*, void*, void*, void*, void*, void*, void*, void*, void* arg9) {
        ITrampoline* t = (ITrampoline*)arg9;
        t->Run();
    }

    void MachineContext::Setup(StackView stack, ITrampoline* trampoline) {
        rsp_ = SetupMachineContext((void*)&stack.back(), (void*)MachineContextTrampoline, (void*)trampoline);
    }

    void MachineContext::SwitchTo(MachineContext& target) {
        SwitchMachineContext(&rsp_, &target.rsp_);
    }

}  // namespace DummyConcurrency::Context
