#pragma once

#include <stdint.h>

enum {
    GDT_ACCESS_PRESENT = 1 << 7,
    GDT_ACCESS_NOT_SYS = 1 << 4,
    GDT_ACCESS_EXEC = 1 << 3,
    GDT_ACCESS_DC = 1 << 2,
    GDT_ACCESS_RW = 1 << 1,
    GDT_ACCESS_ACCESSED = 1 << 0,
};

enum {
    // granularity bit
    // if set segment limit is interpreted as 4kb units
    // else byte
    GDT_FLAGS_GRAN_4K = 1 << 7,
    GDT_FLAGS_SZ_32 = 1 << 6,
    GDT_FLAGS_LONG_MODE = 1 << 5
};

struct gdt_64 {
    uint64_t null;
    // Refer to 4.8.1 AMD64 Architecture programmers manual
    // when it says (ignored) it means (ignored in 64 bit mode)
    struct {
        // Segment limit (16 bits, 0-15) (ignored)
        uint16_t segment_limit_low;
        // Base Address (16 bits, 16-31) (ignored)
        uint16_t base_address_low;

        // Base address (8 bits, 24-31) (ignored)
        uint8_t base_address_mid;

        // access (8 bits, 8-15)
        // (ignored)
        uint8_t access_accessed : 1;
        // (ignored) readable
        uint8_t access_readable : 1;

        uint8_t access_confirming : 1;
        // must be 1 for code
        uint8_t access_executable : 1;
        // must always be set to 1 (ignored)
        // (has a name I think but idk) (sys something?)
        uint8_t access_reserved_one : 1;

        uint8_t access_descriptor_privilege : 2;
        uint8_t access_present : 1;

        // Segment limit (4 bits, 16-19) (ignored)
        uint8_t segment_limit_high : 4;

        // flags (4 bits, 20-23)
        // (ignored)
        uint8_t flags_available : 1;
        uint8_t flags_long_mode : 1;
        uint8_t flags_db : 1;
        // (ignored)
        uint8_t flags_granularity : 1;
        // (ignored)
        uint8_t base_address_high;
    } code;
    // Refer to 4.8.2 (same book described above)
    struct {
        // Segment limit (8 bits, 0-15) (ignored)
        uint16_t segment_limit_low;
        // (ignored)
        uint16_t base_address_low;
        // (ignored)
        uint8_t base_address_mid;
        // access - EVERYTHING IS IGNORED EXCEPT access_p
        uint8_t access_accessed : 1;
        uint8_t access_writable : 1;
        uint8_t access_expand_down : 1;
        // must be 0 (I think this is a executable flag?)
        uint8_t access_reserved_zero : 1;
        // must be 1 (has a name I think but idk)
        uint8_t access_reserved_one : 1;
        uint8_t access_descriptor_privilege_level : 2;
        uint8_t access_present : 1;
        // (ignored)
        uint8_t segment_limit_high : 4;
        // flags (all ignored)
        uint8_t flags_available : 1;
        uint8_t flags_unused : 1;
        uint8_t flags_db : 1;
        uint8_t flags_granularity : 1;

        // (ignored)
        uint8_t base_address_high;
    } data;
    // I give up
    // Magic numbers will be set here
    struct {
        uint32_t low;
        uint32_t high;
    } tss;
} __attribute__((packed));

void enter_long_mode(void* main_function);
