#pragma once

#include <stdint.h>
#include <stdbool.h>

// Checks whether apic is supported
bool apic_is_supported(void);

// Gets the local apic id
int apic_get_id(void);

// Enables apic
void enable_apic(void);

// Read apic register
uint32_t read_reg(void *const ioapicaddr, uint32_t reg);

// Write to apic register
void write_reg(void *const ioapicaddr, uint32_t reg, uint32_t value);

// Signal end of interrupt to apic
void send_apic_eoi(void);

// Initialises the apic timer
void init_apic_timer(uint32_t initial_count, uint8_t vector);
