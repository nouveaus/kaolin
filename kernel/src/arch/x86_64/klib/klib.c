#include "klib.h"

#include "../drivers/timer/timer.h"

void ksleep(int ticks) {
    apic_timer_init(ticks, 0x20);
    while (apic_timer_get_current());
}

_Noreturn void _die(void) {
    while (1) {
        asm volatile("cli\nhlt" ::);
    }
}
