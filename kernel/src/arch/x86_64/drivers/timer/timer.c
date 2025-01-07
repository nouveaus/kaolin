#include "timer.h"

#include "../../apic/lapic.h"

static int timer_ticks = 0;

uint32_t get_timer_ticks(void) {
    return timer_ticks;
}

void timer_handler(void) {
    //asm volatile ("pusha\n");
    send_apic_eoi();
    asm volatile (/*"popa\n*/"leave\niretq");
}
