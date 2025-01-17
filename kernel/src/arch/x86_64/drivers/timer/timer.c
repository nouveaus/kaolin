#include "timer.h"

#include "../../apic/lapic.h"

static int timer_ticks = 0;

uint32_t get_timer_ticks(void) {
    return timer_ticks;
}

__attribute__((interrupt)) void timer_handler(struct interrupt_frame *frame) {
    send_apic_eoi();
}
