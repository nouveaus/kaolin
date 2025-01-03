#include "memmap.h"

#include "../io/io.h"

#include <stddef.h>

void memmap_print_entries(
    uint32_t entry_count,
    struct address_range_descriptor address_range_descriptor[]) {
    for (size_t i = 0; i < entry_count; i++) {
        krintf(
            "entry count %d\n"
            "base address lower: %x, base address higher: %x\n"
            "length lower: %x, length higher: %x\n"
            "type: %d, extended attributes: %x\n",
            i, address_range_descriptor[i].base_address_lower,
            address_range_descriptor[i].base_address_higher,
            address_range_descriptor[i].length_lower,
            address_range_descriptor[i].length_higher,
            address_range_descriptor[i].type,
            address_range_descriptor[i].extended_attributes);
    }
}