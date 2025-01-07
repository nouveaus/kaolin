#pragma once

#include <stdint.h>

#define IOAPICBASE 0xFEC00000

// Reads register from ioapic
uint32_t ioapic_read_reg(void *ioapicaddr, const uint32_t reg);

// Writes register to ioapic
void ioapic_write_reg(void *ioapicaddr, const uint32_t reg, const uint32_t value);

// Redirect a hardware interrupt to a local apic
void ioapic_set_redirect(void *ioapicaddr, const uint8_t vector,
                         const uint8_t apic_id);
