.global SetupMachineContext
.global SwitchMachineContext

# https://eli.thegreenplace.net/2011/02/04/where-the-top-of-the-stack-is-on-x86/

# SetupMachineContext(stack, trampoline, arg)

SetupMachineContext:
    # Arguments
    # 1) %rcx - top of the stack
    # 2) %rdx - bottom of the stack
    # 3) %R8 - trampoline
    # 4) %R9 - trampoline arg

    # Switch to stack
    movq %rsp, %r11 # tmp := rsp
    movq %rcx, %rsp # rsp := rcx

    # Just to be safe
    subq $64, %rsp

    # trampoline stack must be 16-byte aligned
    andq $-16, %rsp

    # 5-th argument for trampoline
    movq %r9, 48(%rsp)

    # Prepare for context switch
    subq $0x120, %rsp
    movq %r8, 0x00(%rsp) # Return address for SwitchMachineContext
    stmxcsr  0x08(%rsp) /* MMX control- and status-word */
    fnstcw   0x0C(%rsp) /* x87 control-word */
    # 2) Callee-saved registers without default values
    # 0x10 r15
    # 0x18 r14
    # 0x20 r13
    # 0x28 r12
    # 0x30 rbx
    # 0x38 rbp
    # 0x40 rdi
    # 0x48 rsi
    # 0x50 %xmm6
    # 0x60 %xmm7
    # 0x70 %xmm8
    # 0x80 %xmm9
    # 0x90 %xmm10
    # 0xA0 %xmm11
    # 0xB0 %xmm12
    # 0xC0 %xmm13
    # 0xD0 %xmm14
    # 0xE0 %xmm15
    # 3) NT_TIB exception stuff
    movq $0, 0x100(%rsp) # Fiber Local Storage, we dont care about it
    movq %rdx, 0x108(%rsp) # Bottom of stack allocation
    movq %rdx, 0x110(%rsp) # Bottom of available stack
    movq %rcx, 0x118(%rsp) # Top of usable stack

    # Return value for SetupMachineContext
    movq %rsp, %rax
    # Restore callstack
    movq %r11, %rsp # rsp := tmp

    retq


SwitchMachineContext:
    movq (%rsp), %rax # Save return address

    subq $0x120, %rsp
    movq %rax, 0x00(%rsp) # Return address for SwitchMachineContext
    stmxcsr  0x08(%rsp) /* save MMX control- and status-word */
    fnstcw   0x0C(%rsp) /* save x87 control-word */
    # 2) Callee-saved registers default values
    movq %r15,  0x10(%rsp)
    movq %r14,  0x18(%rsp)
    movq %r13,  0x20(%rsp)
    movq %r12,  0x28(%rsp)
    movq %rbx,  0x30(%rsp)
    movq %rbp,  0x38(%rsp)
    movq %rdi,  0x40(%rsp)
    movq %rsi,  0x48(%rsp)
    movups %xmm6, 0x50(%rsp)
    movups %xmm7, 0x60(%rsp)
    movups %xmm8, 0x70(%rsp)
    movups %xmm9, 0x80(%rsp)
    movups %xmm10, 0x90(%rsp)
    movups %xmm11, 0xA0(%rsp)
    movups %xmm12, 0xB0(%rsp)
    movups %xmm13, 0xC0(%rsp)
    movups %xmm14, 0xD0(%rsp)
    movups %xmm15, 0xE0(%rsp)
    # 3) exception stuff

    /* Save NT_TIB */
    movq  %gs:(0x30), %r10
    movq  0x20(%r10), %r11
    movq  %r11, 0x100(%rsp)
    movq  0x1478(%r10), %r11
    movq  %r11, 0x108(%rsp)
    movq  0x10(%r10), %r11
    movq  %r11, 0x110(%rsp)
    movq  0x08(%r10), %r11
    movq  %r11, 0x118(%rsp)
    
    # 1.2 Save current stack pointer to 'from' MachineContext
    movq %rsp, (%rcx) # from_rsp := rsp

    # 2. Activate 'to' machine context

    # 2.1 Set stack pointer to target stack
    movq (%rdx), %rsp # rsp := to_rsp
    # 2.2 Restore and pop registers saved on target stack
    movq 0x00(%rsp), %rax
    ldmxcsr 0x08(%rsp) /* save MMX control- and status-word */
    fldcw   0x0C(%rsp) /* save x87 control-word */

    movq 0x10(%rsp), %r15  
    movq 0x18(%rsp), %r14  
    movq 0x20(%rsp), %r13  
    movq 0x28(%rsp), %r12  
    movq 0x30(%rsp), %rbx  
    movq 0x38(%rsp), %rbp  
    movq 0x40(%rsp), %rdi  
    movq 0x48(%rsp), %rsi  
    movups 0x50(%rsp), %xmm6 
    movups 0x60(%rsp), %xmm7 
    movups 0x70(%rsp), %xmm8 
    movups 0x80(%rsp), %xmm9 
    movups 0x90(%rsp), %xmm10 
    movups 0xA0(%rsp), %xmm11 
    movups 0xB0(%rsp), %xmm12 
    movups 0xC0(%rsp), %xmm13 
    movups 0xD0(%rsp), %xmm14 
    movups 0xE0(%rsp), %xmm15

    /* Restore NT_TIB */
    movq  %gs:(0x30), %r10
    movq  0x100(%rsp), %r11
    movq  %r11, 0x20(%r10)
    movq  0x108(%rsp), %r11
    movq  %r11, 0x1478(%r10)
    movq  0x110(%rsp), %r11
    movq  %r11, 0x10(%r10)
    movq  0x118(%rsp), %r11
    movq  %r11, 0x08(%r10)

    addq $0x120, %rsp // 16 * 10 bytes for xmm6-xmm15

    movq %rax, (%rsp)

    retq
