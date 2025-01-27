#include "klib.h"

#include "../../../io.h"

__attribute__((interrupt)) void trap(struct interrupt_frame *frame) {
    // rax is the system call number
    // args are in rdi, rsi, rdx

    uint64_t system_call;
    uint64_t arg_1;
    uint64_t arg_2;
    uint64_t arg_3;

    asm volatile("mov %%rax, %0\n"
                 "mov %%rdi, %1\n"
                 "mov %%rsi, %2\n"
                 "mov %%rdx, %3\n"
                 : "=r"(system_call), "=r"(arg_1), "=r"(arg_2), "=r"(arg_3)
                 :);

    switch (system_call) {
        case 0:
            puts((const char *) arg_1);
            break;
        case 1:
            putc((char) (arg_1 & 0xFF));
            break;
        case 2:
            puti((int) (arg_1 & 0xFFFFFFFF));
            break;
        default:
            // todo: return a sentinel value to indicate invalids
            puts("Invalid system call!\n");
    }


    //puts("Trap\n");
}

__attribute__((interrupt)) void exception_handler(struct interrupt_frame *frame) {
    krintf("Fatal Error Occurred! Error Code: %x\n", frame->one);
    while (1) asm volatile("cli\nhlt" ::);
}
