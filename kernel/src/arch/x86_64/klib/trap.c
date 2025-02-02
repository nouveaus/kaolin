#include "klib.h"

#include "../../../io.h"

void trap(struct interrupt_frame *frame) {
    // rax is the system call number
    // args are in rdi, rsi, rdx

    register uint64_t system_call asm("rax");
    register uint64_t arg_1 asm("rdi");
    register uint64_t arg_2 asm("rsi");
    register uint64_t arg_3 asm("rdx");

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
            asm volatile("mov $0, %%rax\n" : : "r"((uint64_t) -1));
    }
}

void exception_handler(struct interrupt_frame *frame) {
    puts("Exception Occurred!\n");
    krintf("Error Code/RIP: %x\n", frame->six);
    krintf("RIP/CS:         %x\n", frame->five);
    krintf("CS/RFLAGS:      %x\n", frame->four);
    krintf("RFLAGS/RSP:     %x\n", frame->three);
    krintf("RSP/SS:         %x\n", frame->two);
    krintf("SS:             %x\n", frame->one);

    _die();
}
