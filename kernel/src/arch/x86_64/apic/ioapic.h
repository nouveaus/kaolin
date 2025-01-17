#pragma once

#include <stdint.h>
#include "../memory/paging.h"

#define IOAPICBASE 0xFEC00000
#define IOAPIC_VIRTUAL_ADDRESS (KERNEL_MAPPING_ADDRESS | IOAPICBASE)

// Reads register from ioapic
uint32_t ioapic_read_reg(void *ioapicaddr, uint32_t reg);

// Writes register to ioapic
void ioapic_write_reg(void *ioapicaddr, uint32_t reg, uint32_t value);

// Maps ioapic to a virtual address
bool ioapic_map(void);

// Redirect a hardware interrupt to a local apic
void ioapic_set_redirect(void *ioapicaddr, uint8_t irq, uint8_t vector, uint8_t apic_id);
