#pragma once

#include "memory/memmap.h"
#include <stdint.h>

// Boot parameters, passed by the bootloader, or shim
struct boot_parameters {
    uint64_t *pml4;
    // Address range descriptors. This could have been collected through int 0x15, E820 in the PC BIOS
    uint32_t address_range_count;
    struct address_range_descriptor *address_ranges;
};

void _Noreturn kernel_main(struct boot_parameters parameters) __attribute__((section(".text.kernel_main"), cdecl));
