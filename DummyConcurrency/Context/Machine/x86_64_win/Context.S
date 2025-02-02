.global SetupMachineContext
.global (SwitchMachineContext

# https://eli.thegreenplace.net/2011/02/04/where-the-top-of-the-stack-is-on-x86/

# SetupMachineContext(stack, trampoline, arg)

ASM_SYMBOL(SetupMachineContext):
    # Arguments
    # 1) %rcx - bottom of the stack
    # 2) %rdx - trampoline
    # 3) %R8 - arg

    # Switch to stack
    movq %rsp, %r11 # tmp := rsp
    movq %rcx, %rsp # rsp := rdi

    # Just to be safe
    subq $64, %rsp

    # trampoline stack must be 16-byte aligned
    andq $-16, %rsp
    addq $8, %rsp

    # 5-th argument for trampoline
    movq %r8, 40(%rsp)

    # Prepare for context switch

    # 1) Return address for SwitchMachineContext
    pushq %rdx

    # 2) Callee-saved registers
    pushq $0 # r15
    pushq $0 # r14
    pushq $0 # r13
    pushq $0 # r12
    pushq $0 # rbx
    pushq $0 # rbp
    pushq $0 # rdi
    pushq $0 # rsi
    
    subq $160, %rsp // 16 * 10 bytes for xmm6-xmm15
    movups %xmm6, 16*0(%rsp)
    movups %xmm7, 16*1(%rsp)
    movups %xmm8, 16*2(%rsp)
    movups %xmm9, 16*3(%rsp)
    movups %xmm10, 16*4(%rsp)
    movups %xmm11, 16*5(%rsp)
    movups %xmm12, 16*6(%rsp)
    movups %xmm13, 16*7(%rsp)
    movups %xmm14, 16*8(%rsp)
    movups %xmm15, 16*9(%rsp)

    # Return value for SetupMachineContext
    movq %rsp, %rax

    # Restore callstack
    movq %r11, %rsp # rsp := tmp

    retq


# SwitchMachineContext(from_rsp, to_rsp)

ASM_SYMBOL(SwitchMachineContext):
    # SwitchMachineContext frame created on top of the current stack

    # 1. Save current execution context to 'from'

    # 1.1 Save callee-saved registers on top of the current stack

    # https://stackoverflow.com/questions/18024672/what-registers-are-preserved-through-a-linux-x86-64-function-call
    # https://uclibc.org/docs/psABI-x86_64.pdf

    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    pushq %rbx
    pushq %rbp
    pushq %rdi
    pushq %rsi
    
    subq $160, %rsp // 16 * 10 bytes for xmm6-xmm15
    movups %xmm6, 16*0(%rsp)
    movups %xmm7, 16*1(%rsp)
    movups %xmm8, 16*2(%rsp)
    movups %xmm9, 16*3(%rsp)
    movups %xmm10, 16*4(%rsp)
    movups %xmm11, 16*5(%rsp)
    movups %xmm12, 16*6(%rsp)
    movups %xmm13, 16*7(%rsp)
    movups %xmm14, 16*8(%rsp)
    movups %xmm15, 16*9(%rsp)

    # Switch stacks

    # 1.2 Save current stack pointer to 'from' MachineContext
    movq %rsp, (%rcx) # from_rsp := rsp

    # 2. Activate 'to' machine context

    # 2.1 Set stack pointer to target stack
    movq (%rdx), %rsp # rsp := to_rsp

    # 2.2 Restore and pop registers saved on target stack

    movups 16*0(%rsp), %xmm6
    movups 16*1(%rsp), %xmm7
    movups 16*2(%rsp), %xmm8
    movups 16*3(%rsp), %xmm9
    movups 16*4(%rsp), %xmm10
    movups 16*5(%rsp), %xmm11
    movups 16*6(%rsp), %xmm12
    movups 16*7(%rsp), %xmm13
    movups 16*8(%rsp), %xmm14
    movups 16*9(%rsp), %xmm15
    
    addq $160, %rsp // 16 * 10 bytes for xmm6-xmm15

    popq %rsi
    popq %rdi
    popq %rbp
    popq %rbx

    popq %r12
    popq %r13
    popq %r14
    popq %r15

    # Pop current SwitchMachineContext frame from target stack

    retq
