#if (APPLE)
  #define ASM_SYMBOL(name) _##name
#else
  #define ASM_SYMBOL(name) name
#endif

.p2align 4

.global ASM_SYMBOL(SetupMachineContext)
.global ASM_SYMBOL(SwitchMachineContext)

# SetupMachineContext(stack, trampoline, arg)

ASM_SYMBOL(SetupMachineContext):
    # Arguments
    # 1) x0 - bottom of the stack
    # 2) x1 - trampoline
    # 3) x2 - arg

    # Switch to stack
    mov     x3, sp
    mov     sp, x0

    # Just to be safe
    sub     sp, sp, #64

    # trampoline stack must be 16-byte aligned
    mov     x4, sp
    and     x4, x4, #-16
    mov     sp, x4

    add     sp, sp, #16

    # 9-th argument for trampoline
    str     x2, [sp, #-16]!

    # 1) Return address for SwitchMachineContext
    str     x1, [sp, #-16]!

    # 2) Callee-saved registers
    mov     x4, #0
    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!

    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!
    stp     x4, x4, [sp, #-16]!

    # Return value for SetupMachineContext
    mov     x0, sp

    # Restore callstack
    mov     sp, x3

    ret

# SwitchMachineContext(from_rsp, to_rsp)

ASM_SYMBOL(SwitchMachineContext):
    # SwitchMachineContext frame created on top of the current stack

    # 1. Save current execution context to 'from'

    # 1.1 Save callee-saved registers on top of the current stack

    # https://documentation-service.arm.com/static/613a2c38674a052ae36ca307
    # https://developer.arm.com/documentation/den0024/a/The-ABI-for-ARM-64-bit-Architecture/Register-use-in-the-AArch64-Procedure-Call-Standard/Parameters-in-general-purpose-registers
  
    stp     x30, x29, [sp, #-16]!
    stp     x20, x19, [sp, #-16]!
    stp     x22, x21, [sp, #-16]!
    stp     x24, x23, [sp, #-16]!
    stp     x26, x25, [sp, #-16]!
    stp     x28, x27, [sp, #-16]!

    stp     d8,  d9,  [sp, #-16]!
    stp     d10, d11, [sp, #-16]!
    stp     d12, d13, [sp, #-16]!
    stp     d14, d15, [sp, #-16]!

    # Switch to stack

    mov     x2, sp
    str     x2, [x0]

    # 1.2 Save current stack pointer to 'from' MachineContext

    ldr     x2, [x1]

    # 2. Activate 'to' machine context

    # 2.1 Set stack pointer to target stack
    mov     sp, x2

    # 2.2 Restore and pop registers saved on target stack
    ldp     d14, d15, [sp], #16
    ldp     d12, d13, [sp], #16
    ldp     d10, d11, [sp], #16
    ldp     d8,  d9,  [sp], #16
  
    ldp     x28, x27, [sp], #16
    ldp     x26, x25, [sp], #16
    ldp     x24, x23, [sp], #16
    ldp     x22, x21, [sp], #16
    ldp     x20, x19, [sp], #16
    ldp     x30, x29, [sp], #16

    # Pop current SwitchMachineContext frame from target stack
    
    ret