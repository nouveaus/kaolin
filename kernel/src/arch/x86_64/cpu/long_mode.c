#include "long_mode.h"

#include <stdint.h>
#include <stddef.h>

#include "cpuid.h"
#include "msr.h"

bool long_mode_is_supported(void) {
    // checks if the function to check its supported
    // exists
    uint32_t eax, ebx, ecx, edx;
    __cpuid(0x80000000, eax, ebx, ecx, edx);
    if (eax < 0x80000001) return false;

    // detects long mode
    __cpuid(0x80000001, eax, ebx, ecx, edx);
    return (edx & (1 << 29)) != 0;
}

void enable_long_mode(void) {
    uint32_t ecx = 0xC0000080, eax, unused;
    cpu_get_msr(ecx, &eax, &unused);
    eax |= 1 << 8;
    cpu_set_msr(ecx, eax, 0);
}

void setup_paging(void) {
// 4096 dwords = 16384 bytes (size of tables)
#define PAGING_TABLE_SIZE 4096
    // sets the paging address
    asm volatile("mov %0, %%cr3\n" : : "r"((uint64_t)PAGING_TABLE_START_ADDRESS));
    uint32_t *paging_table = (uint32_t*)PAGING_TABLE_START_ADDRESS;
    // clear out space
    for (size_t i = 0; i < PAGING_TABLE_SIZE; i++) {
        paging_table[i] = 0;
    }

    // for now we only do 2 mb
    uint32_t *PM4T = (uint32_t*)PML4T_START_ADDRESS;
    PM4T[0] = 0x2003;
    uint32_t *PDPT = (uint32_t*)PDPT_START_ADDRESS;
    PDPT[0] = 0x3003;
    uint32_t *PDT = (uint32_t*)DPT_START_ADDRESS;
    PDT[0] = 0x4003;
    uint32_t *PT = (uint32_t*)PT_START_ADDRESS;

    // Refer to page 148 for entry structure in each table in
    // AMD64 architecture programmers manual

    // Each entry is 64 bits (split into two uint32_t)

    // 0x3 sets read/write flag and present flag
    // present flag indicates its present in memory
    // read/write indicates its writable
    
    // TODO: replace constant with flags (enum)
    uint32_t address = 0x00000003;
    // maps first 2 mb
    for (size_t i = 0; i < 512; i++) {
        // always set the lower 32 bits
        // 0x1000 is used as it accesses the 13 bit
        PT[i * 2] = address;
        address += 0x1000;
    }
}
