#include "klib.h"

#include "../apic/lapic.h"
#include "../../../io.h"

void ksleep(int ticks) {
    init_apic_timer(ticks, 0x20);
    while (get_apic_timer_current());
}

void trap(void) {
    asm volatile ("pusha\n");
    puts("Trap\n");
    asm volatile ("popa\nleave\niret");
}


