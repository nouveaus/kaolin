#include "memmap.h"

#include "../../../io.h"
#include "../klib/klib.h"

#include <stddef.h>

void memmap_print_entries(struct address_ranges address_ranges) {
    for (size_t i = 0; i < address_ranges.count; i++) {
        struct address_range_descriptor entry = address_ranges.entries[i];
        krintf(
                "entry count %d\n"
                "base address lower: %x, base address higher: %x\n"
                "length lower: %x, length higher: %x\n"
                "type: %d, extended attributes: %x\n",
                i,
                entry.base_address_lower,
                entry.base_address_higher,
                entry.length_lower,
                entry.length_higher,
                entry.type,
                entry.extended_attributes);
        ksleep(276447232);
    }
}
