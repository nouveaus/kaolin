#include "klib.h"

#include "../../../io.h"

__attribute__((interrupt)) void trap(struct interrupt_frame* frame) {
    puts("Trap\n");
}
