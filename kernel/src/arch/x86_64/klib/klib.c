#include "klib.h"

#include "../apic/lapic.h"

void ksleep(int ticks) {
    init_apic_timer(ticks, 0x20);
    while (get_apic_timer_current());
}