#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "../memory/paging.h"

#define BASE_LAPIC            0xFEE00000
#define LAPIC_VIRTUAL_ADDRESS (KERNEL_MAPPING_ADDRESS | BASE_LAPIC)

// Checks whether apic is supported
bool apic_is_supported(void);

// Gets the local apic id
int apic_get_id(void);

/// Maps apic to virtual memory address
bool map_apic(void);

// Enables apic
void enable_apic(void);

// Read apic register
uint32_t read_reg(void *ioapicaddr, uint32_t reg);

// Write to apic register
void write_reg(void *ioapicaddr, uint32_t reg, uint32_t value);

// Signal end of interrupt to apic
void send_apic_eoi(void);

