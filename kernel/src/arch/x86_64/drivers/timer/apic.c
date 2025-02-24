#include "timer.h"

#include "../../apic/lapic.h"

static uint32_t timer_ticks = 0;

// Refer to Intel IA-32 Volume 3 12.5.4 APIC Timer
void apic_timer_init(uint32_t initial_count, uint8_t vector) {
    void *lapic = apic_get_address();
    // be divisble by 1 (every 10ms) (Figure 12-10)
    *(uint32_t volatile *) ((uint64_t) lapic + 0x3E0) = 1011;
    // set initial count (Figure 12-11)
    *(uint32_t volatile *) ((uint64_t) lapic + 0x380) = initial_count;
// Refer to 12.5.1 (Timer mode and vector in Figure 12-8)
// 0 = one shot, 1 = periodic, 2 = TSC-deadline
#define APIC_TIMER_MODE 0
    *(uint32_t volatile *) ((uint64_t) lapic + 0x320) =
            vector | (APIC_TIMER_MODE << 17);
}

// returns time left from countdown iirc
uint32_t apic_timer_get_current(void) {
    void *lapic = apic_get_address();
    return *(uint32_t volatile *) ((uint64_t) lapic + 0x390);
}


uint32_t apic_timer_get_ticks(void) {
    return timer_ticks;
}

__attribute__((interrupt)) void apic_timer_handler(struct interrupt_frame *frame) {
    timer_ticks++;
    send_apic_eoi();
}
