#pragma once

#include <stdint.h>
#include <stdbool.h>

#define IOAPICBASE 0xFEC00000

// Checks whether apic is supported
bool apic_is_supported(void);

// Enables apic
void enable_apic(void);

// Read apic register
uint32_t read_reg(const void *ioapicaddr, const uint32_t reg);

// Write to apic register
void write_reg(const void *ioapicaddr, const uint32_t reg, const uint32_t value);