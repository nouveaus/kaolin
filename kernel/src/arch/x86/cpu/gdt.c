#include "gdt.h"

// any field not mentioned is initialised to 0
struct gdt_64 gdt = {
    .code =
        {
            .segment_limit_low = 0xFFFF,
            .access_present = 1,
            .access_reserved_one = 1,
            .access_executable = 1,
            .access_readable = 1,

            .segment_limit_high = 0xF,

            .flags_granularity = 1,
            .flags_long_mode = 1,
        },
    .data =
        {
            .segment_limit_low = 0xFFFF,
            .access_present = 1,
            .access_reserved_one = 1,
            .access_writable = 1,

            .segment_limit_high = 0xF,

            .flags_granularity = 1,
            .flags_db = 1  // SZ_32 in the osdev examples
        },
    .tss =
        {
            .low = 0x00000068,
            .high = 0x00CF8900,
        },
};

struct gdt_64_pointer {
    uint16_t size;
    uint64_t address;
} __attribute__((packed));

void enter_long_mode(void* main_function) {
    struct gdt_64_pointer pointer = {.size = sizeof(gdt) - 1,
                                     .address = (uint64_t)&gdt};
    asm volatile("lgdt %0" ::"m"(pointer));
    // Load data descriptor
    asm volatile(
        "cli\n"
        "mov %0, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n" ::"m"(gdt.data));
    
    // perform jump here
    asm volatile(
        // pushes code segment selector (offset to code in gdt)
        "pushq  $0x08\n"
        "pushq %0\n"
        // pops 2 and performs long jump
        "lretq\n" ::"r"((uint64_t)(main_function))
        : "memory");
}
