#pragma once

#include <stdbool.h>
#include <stdint.h>

#define COMMON_HEADER        \
    uint16_t vendor_ID;      \
    uint16_t device_ID;      \
    uint16_t command;        \
    uint16_t status;         \
    uint8_t revision_ID;     \
    uint8_t prog_IF;         \
    uint8_t subclass;        \
    uint8_t class_code;      \
    uint8_t cache_line_size; \
    uint8_t latency_timer;   \
    uint8_t header_type;     \
    uint8_t BIST;


struct common_header {
    COMMON_HEADER
    //    uint8_t data[];
} __attribute__((packed));

struct device_header {
    COMMON_HEADER
    uint32_t base_addresses[6];
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor_ID;
    uint16_t subsystem_device_ID;
    uint32_t expansion_rom_address;
    uint8_t capabilities_pointer;
    uint8_t reserved0;
    uint16_t reserved1;
    uint32_t reserved2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} __attribute__((packed));

// pci to pci
struct pci_bridge_header {
    COMMON_HEADER
    uint32_t base_addresses[2];
    uint8_t primary_bus_number;
    uint8_t secondary_bus_number;
    uint8_t subordinate_bus_number;
    uint8_t secondary_latency_timer;
    uint8_t io_base;
    uint8_t io_limit;
    uint8_t secondary_status;
    uint16_t memory_base;
    uint16_t memory_limit;
    uint16_t prefetchable_memory_base;
    uint16_t prefetchable_memory_limit;
    uint32_t prefetchable_memory_base_upper;
    uint32_t prefetchable_memory_limit_upper;
    uint16_t io_base_upper;
    uint16_t io_limit_upper;
    uint8_t capabilities_pointer;
    uint8_t reserved0;
    uint16_t reserved1;
    uint32_t expansion_ROM_base_address;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
} __attribute__((packed));

// pci to cardbus
struct cardbus_bridge_header {
    COMMON_HEADER
    uint32_t base_address;
    uint8_t offset_capabilities_list;
    uint8_t reserved0;
    uint16_t secondary_status;
    uint8_t pci_bus_number;
    uint8_t cardbus_bus_number;
    uint8_t subordinate_bus_number;
    uint8_t cardbus_latency_timer;
    uint32_t memory_base_address0;
    uint32_t memory_limit0;
    uint32_t memory_base_address1;
    uint32_t memory_limit1;
    uint32_t io_base_address0;
    uint32_t io_limit0;
    uint32_t io_base_address1;
    uint32_t io_limit1;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint16_t bridge_control;
    uint16_t subsystem_device_ID;
    uint16_t subsystem_vendor_ID;
    // 16 bit pc card
    uint32_t legacy_mode_base_address;
} __attribute__((packed));

uint32_t pci_config_readl(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

void pci_scan(void);

struct common_header pci_get_config(uint8_t bus, uint8_t slot, uint8_t func);

void pci_check_slot(uint8_t bus, uint8_t slot);

void pci_init(void);

bool pci_msi_supported(void);
