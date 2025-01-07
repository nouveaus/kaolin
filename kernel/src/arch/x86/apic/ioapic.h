#pragma once

#include <stdint.h>

#define IOAPICBASE 0xFEC00000

// Reads register from ioapic
uint32_t ioapic_read_reg(void *ioapicaddr, const uint32_t reg);

// Writes register to ioapic
void ioapic_write_reg(void *ioapicaddr, const uint32_t reg, const uint32_t value);

// Redirect a hardware interrupt to a local apic
<<<<<<< HEAD
void ioapic_set_redirect(void *ioapicaddr, const uint8_t vector,
                         const uint8_t apic_id);
=======
void ioapic_set_redirect(void *ioapicaddr, const uint8_t irq, const uint8_t vector, const uint8_t apic_id);
>>>>>>> 4aafccfcb98e5d217b9aeffdc8f95d934b4a10c9
