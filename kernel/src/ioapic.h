#pragma once

#include <stdint.h>

#define IOAPICBASE 0xFEC00000

// Reads register from ioapic
uint32_t ioapic_read_reg(const uintptr_t *ioapicaddr, const uint32_t reg);

// Writes register to ioapic
void ioapic_write_reg(const uintptr_t *ioapicaddr, const uint32_t reg, const uint32_t value);

// Redirect a hardware interrupt to a local apic
void ioapic_set_redirect(const uintptr_t *ioapicaddr, const uint32_t vector,
                         const uint32_t apic_id);