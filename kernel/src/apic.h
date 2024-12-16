#pragma once

#include <stdint.h>
#include <stdbool.h>

#define IOAPICBASE 0xFEC00000

bool apic_is_supported(void);
void enable_apic(void);

uint32_t cpu_read_to_apic(const void *ioapicaddr, const uint32_t reg);
void cpu_write_to_apic(const void *ioapicaddr, const uint32_t reg, const uint32_t value);
uint32_t read_reg(const void *ioapicaddr, const uint32_t reg);
void write_reg(const void *ioapicaddr, const uint32_t reg, const uint32_t value);