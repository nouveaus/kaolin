#pragma once

#include "memory/memmap.h"

// Boot parameters, passed by the bootloader, or shim
struct boot_parameters {
    // Address range descriptors. This could have been collected through int 0x15, E820 in the PC BIOS
    struct address_ranges address_ranges;
};

void _Noreturn kernel_main(struct boot_parameters parameters) __attribute__((section(".text.kernel_main")));
