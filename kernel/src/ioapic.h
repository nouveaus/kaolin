#pragma once

#include <stdint.h>

#define IOAPICBASE 0xFEC00000

uint32_t ioapic_read_reg(const uintptr_t *ioapicaddr, const uint32_t reg);
void ioapic_write_reg(const uintptr_t *ioapicaddr, const uint32_t reg, const uint32_t value);
void ioapic_set_redirect(const uintptr_t *ioapicaddr, const uint32_t vector,
                         const uint32_t apic_id);