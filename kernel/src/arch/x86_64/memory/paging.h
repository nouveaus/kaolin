#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// all tables basically share
// the same format entry
// Refer to Figure 5-19 from AMD Programmers manual

// not used
struct page_entry {
    uint64_t present : 1;
    uint64_t read_write : 1;
    uint64_t user : 1;
    uint64_t pwt : 1;
    uint64_t pcd : 1;
    uint64_t accessed : 1;
    uint64_t dirty : 1;
    uint64_t pat : 1;
    uint64_t global : 1;
    uint64_t avl : 1;
    uint64_t address : 40;
    uint64_t available : 11;
    uint64_t nx : 1;
} __attribute__((packed));

#define PAGE_PRESENT 0x1
#define PAGE_WRITE   0x2
#define PAGE_USER    0x4
#define PAGE_CACHE_DISABLE 0x10

// kernel uses the upper address for mmio stuff
// linux does it by  ffff800000000000 - ffffffffffffffff
// https://www.kernel.org/doc/html/v5.8/x86/x86_64/mm.html
#define KERNEL_MAPPING_ADDRESS 0xFFFF8000UL << 32

// Frees a virtual address and frees any pages associated if it has no entries
bool free_address(uint64_t *pml4, uint64_t virtual_address);
// Maps a physical address to a physical address with flags
void map_page(uint64_t *pml4, uint64_t virtual_address,
              uint64_t physical_address, uint16_t flags);
// Verifies that a virtual address has successfully been mapped to a physical address
bool verify_mapping(uint64_t *pml4, uint64_t virtual_address);

// Maps pages depending on the given size
void *mmap(uint64_t *pml4, void *address, size_t size, void **end_address);
