#include "ioapic.h"

#include <stdint.h>

uint32_t ioapic_read_reg(const uintptr_t *ioapicaddr, const uint32_t reg) {
    uint32_t volatile *const ioapic = (uint32_t volatile *const)ioapicaddr;
    ioapic[0] = reg;

    // window of ioapic
    return ioapic[4];
}

void ioapic_write_reg(const uintptr_t *ioapicaddr, const uint32_t reg, const uint32_t value) {
   uint32_t volatile *const ioapic = (uint32_t volatile *const)ioapicaddr;
   ioapic[0] = reg;
   ioapic[4] = value;
}

void ioapic_set_redirect(const uintptr_t *ioapicaddr, const uint32_t vector, const uint32_t apic_id) {
    // Redirects hardware interupts to local apic
    uint8_t low_idx = 0x10 + vector * 2;
    uint8_t high_idx = low_idx + 1;
    uint32_t dest = apic_id << 24;
    ioapic_write_reg(ioapicaddr, low_idx, vector);
    ioapic_write_reg(ioapicaddr, high_idx, dest);
}