#pragma once

#include <stdint.h>

// Shamelessly stolen from: https://wiki.osdev.org/Getting_to_Ring_3
struct gdt_entry {
    // segment limit (limit of the segments size)
    uint64_t limit_low : 16;

    // base address = virtual address to start of segment
    uint64_t base_low : 24;

    uint64_t accessed : 1;
    // code is readable
    // data is writable
    uint64_t read_write : 1;

    // conforming for code, expand down for data
    uint64_t conforming_expand_down : 1;
    // 1 for code, 0 for data
    uint64_t code : 1;

    // should be 1 for everything but TSS and LDT
    uint64_t code_data_segment : 1;

    // privilege level
    uint64_t DPL : 2;
    // indicates that segment referenced by the descriptor is loaded in memory
    uint64_t present : 1;

    uint64_t limit_high : 4;

    // only used in software; has no effect on hardware
    uint64_t available : 1;
    // self explaintory (it enables long mode!!!)
    uint64_t long_mode : 1;
    // 32-bit opcodes for code, uint32_t stack for data
    uint64_t big : 1;
    // 1 to use 4k page addressing, 0 for byte addressing
    uint64_t gran : 1;

    uint64_t base_high : 8;

} __attribute__((packed));

struct gdt_system_entry {
    uint64_t limit_low : 16;
    uint64_t base_low : 24;

    // types
    uint64_t accessed : 1;
    uint64_t read_write : 1;
    uint64_t conforming_expand_down : 1;
    uint64_t code : 1;

    // always 0
    uint64_t code_data_segment : 1;

    uint64_t DPL : 2;
    uint64_t present : 1;
    uint64_t limit_high : 4;
    uint64_t available : 1;
    uint64_t unused : 2;
    uint64_t gran : 1;

    uint64_t base_mid : 8;

    // * end of first 64 bits

    uint64_t base_high : 32;
    uint64_t reserved_low : 8;
    uint64_t must_be_zero : 5;
    uint64_t reerved_high : 19;
} __attribute__((packed));

// Refer to page 377 (Figure 12-8) in AMD manual
struct tss_entry {
    uint32_t reserved_low;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved_mid;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved_high;
    uint16_t reserved_higher;
    uint16_t io_base_address;
} __attribute__((packed));

struct gdt {
    struct gdt_entry null;
    struct gdt_entry kernel_code;
    struct gdt_entry kernel_data;
    struct gdt_entry user_code;
    struct gdt_entry user_data;
    struct gdt_system_entry tss;
} __attribute__((packed));

// Enters usermode
void _Noreturn enter_usermode(void *function_address);
