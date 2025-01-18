#include "klib.h"

#include "../../../io.h"

__attribute__((interrupt)) void trap(struct interrupt_frame *frame) {
    puts("Trap\n");
}

__attribute__((interrupt)) void exception_handler(struct interrupt_frame *frame) {
    krintf("Fatal Error Occurred! Error Code: %x\n", frame->one);
    while (1) asm volatile("cli\nhlt" ::);
}

