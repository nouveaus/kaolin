#include "ioapic.h"

#include <stdint.h>

uint32_t ioapic_read_reg(void *ioapicaddr, uint32_t reg) {
    uint32_t volatile *ioapic = (uint32_t volatile *)ioapicaddr;
    ioapic[0] = reg;

    // window of ioapic
    return ioapic[4];
}

void ioapic_write_reg(void *ioapicaddr, uint32_t reg, uint32_t value) {
   uint32_t volatile *ioapic = (uint32_t volatile *)ioapicaddr;
   ioapic[0] = reg;
   ioapic[4] = value;
}

bool ioapic_map(void) {
    map_page(IOAPIC_VIRTUAL_ADDRESS, IOAPICBASE, PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE);
    return verify_mapping(IOAPIC_VIRTUAL_ADDRESS);
}

void ioapic_set_redirect(void *ioapicaddr, uint8_t irq, uint8_t vector, uint8_t apic_id) {
    // Redirects hardware interupts to local apic
    // Refer to Intel IA-32 Volume 3 12.6.1 Interrupt Command Register
    // This accesses the lower 32 bits
    uint8_t low_idx = 0x10 + irq * 2;

    // This accesses the higher 32 bits
    uint8_t high_idx = low_idx + 1;

    // Destination field occupies 63-56 bits
    uint32_t dest = apic_id << 24;

    // vector is promoted to 32 bits
    // everything else is zeroed out:
    // delivery mode, destination_mode, delivery_status, level, trigger_mode
    ioapic_write_reg(ioapicaddr, low_idx, vector);
    ioapic_write_reg(ioapicaddr, high_idx, dest);
}
