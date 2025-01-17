#pragma once

#include <stdint.h>

// Refer to 15.1 ACPI specification (Table 15.4)
struct address_range_descriptor {
    uint32_t base_address_lower;
    uint32_t base_address_higher;
    uint32_t length_lower;
    uint32_t length_higher;
    uint32_t type;
    uint32_t extended_attributes;
};

// Prints all the entries of the memory map
void memmap_print_entries(
        uint32_t entry_count,
        struct address_range_descriptor address_range_descriptor[]);
