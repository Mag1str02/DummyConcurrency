#include <DummyConcurrency/Context/Machine/ucontext/Context.hpp>

namespace DummyConcurrency::Context {

    /*
     * Workaround for legacy `ucontext` API:
     *
     * makecontext and ucontext trampoline expect arguments of type int even on 64-bit arch,
     * so we have to break down a pointer to ITrampoline to a couple of 32-bit integers
     * (lo and hi part of the address) and pass them to the trampoline separately to avoid UB
     *
     * Actually, any sane implementation of `ucontext` for 64-bit arch will support void*
     * as a trampoline argument, but formally it will still be UB
     */

    // Expectations: 64-bit arch, 4-byte int
    static_assert(sizeof(void*) == 8);
    static_assert(sizeof(int) == 4);

    namespace {

        struct PointerRepr {
            int lo;
            int hi;
        };

        union Pointer {
            PointerRepr repr;
            void*       addr;
        };

    }  // namespace

    static void MachineContextTrampoline(int lo, int hi) {
        Pointer arg{
            .repr = {lo, hi}
        };

        ITrampoline* t = (ITrampoline*)arg.addr;
        t->Run();
    }

    MachineContext::MachineContext() {
        // getcontext(&context);
    }

    typedef void (*UcontextTrampolineType)(void);

    void MachineContext::Setup(StackView stack, ITrampoline* trampoline) {
        getcontext(&context);

        context.uc_link           = 0;
        context.uc_stack.ss_sp    = stack.data();
        context.uc_stack.ss_size  = stack.size();
        context.uc_stack.ss_flags = 0;

        Pointer t{.addr = trampoline};

        makecontext(&context, (UcontextTrampolineType)MachineContextTrampoline, 2, t.repr.lo, t.repr.hi);
    }

    void MachineContext::SwitchTo(MachineContext& target) {
        swapcontext(&context, &target.context);
    }

}  // namespace DummyConcurrency::Context
