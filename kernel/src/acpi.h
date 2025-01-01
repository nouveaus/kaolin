#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Refer to 5.2.5.3 of ACPI Spec
struct rsdp {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    // unused
    uint32_t rsdt_address;
    // length of table
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} __attribute__((packed));

// Refer to 5.2.6 of ACPI Spec
#define DESCRIPTION_HEADER \
    char signature[4];     \
    uint32_t length;       \
    uint8_t revision;      \
    uint8_t checksum;      \
    char OEMID[6];         \
    uint64_t OEM_table_ID; \
    uint32_t OEM_revision; \
    uint32_t creator_ID;   \
    uint32_t creator_revision;

// Refer to 5.2.7 of ACPI Spec
struct rsdt {
    DESCRIPTION_HEADER
    uint32_t entry[];
} __attribute__((packed));

struct description_header {
    DESCRIPTION_HEADER
} __attribute__((packed));

struct madt {
    DESCRIPTION_HEADER
    // local apic
    uint32_t local_interrupt_controller_address;
    uint32_t flags;
} __attribute__((packed));

#define MADT_ENTRY \
    uint8_t type; \
    uint8_t length;

// generalised struct
// every entry of the madt always has type and length
struct madt_entry {
    MADT_ENTRY
    uint8_t data[];
} __attribute__((packed));

struct madt_entry_apicio {
    MADT_ENTRY
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_address;
    uint32_t global_system_interrupt_base;
} __attribute__((packed));

// Finds the rsdp and sets the address
bool rsdp_find(void);

// Verifies the rsdp
bool rsdp_verify(void);

// Gets the revision number of the acpi
uint8_t rsdp_get_revision(void);

// Prints the signature of the rsdp
void rsdp_print_signature(void);

// Verifies the rsdt
bool rsdt_verify(void);

// Finds the madt and sets the address
bool madt_find(void);

// Verifies the madt
bool madt_verify(void);

// Gets the local apic address
uint32_t madt_get_lapic_address(void);

// Gets the count of ioapic and gets the address of the first ioapic
size_t ioapic_get_entry_count(void);

// Returns the first ioapic address
uint32_t get_first_ioapic_address(void);