#include "paging.h"

#define PAGE_SIZE 4096
// technically we have MAX_PAGES + 4 because the + 4 was made in bootloader
#define MAX_PAGES 4096
#define PHYSICAL_MEMORY_START 0x00100000
#define MAX_PAGE_ENTRIES 512

// Inspired by https://wiki.osdev.org/Page_Frame_Allocation (bitmap)
// this keeps track of what page is being used
static uint8_t bitmap[MAX_PAGES / 8] = {0};

bool bitmap_check_page(size_t i) { return (bitmap[i / 8] >> (i % 8)) & 0x1; }

void bitmap_set_page(size_t i) { bitmap[i / 8] |= (1 << (i % 8)); }

void bitmap_clear_page(size_t i) { bitmap[i / 8] &= ~(1 << (i % 8)); }

static void *allocate_page(void) {
    for (size_t i = 0; i < MAX_PAGES; i++) {
        if (!bitmap_check_page(i)) {
            bitmap_set_page(i);
            return (void *)(PHYSICAL_MEMORY_START + i * PAGE_SIZE);
        }
    }
    return NULL;
}

static void free_page(void *page) {
    size_t index = ((uint64_t)page - PHYSICAL_MEMORY_START) / PAGE_SIZE;
    if (index < 0 || index >= MAX_PAGES) return;
    bitmap_clear_page(index);
}

static void clear_page(uint8_t *page) {
    for (size_t i = 0; i < PAGE_SIZE; i++) {
        page[i] = 0;
    }
}

static bool check_page_is_empty(uint64_t *page) {
    for (size_t i = 0; i < MAX_PAGE_ENTRIES; i++) {
        if (page[i]) return false;
    }
    free_page(page);
    return true;
}

// must invalidate tlb (https://wiki.osdev.org/TLB)
// if any changes are made to pages
static void invalidate_tlb(uint64_t virtual_address) {
    asm volatile("invlpg (%0)" ::"r"(virtual_address) : "memory");
}

bool free_address(uint64_t *pml4, uint64_t virtual_address) {
    uint16_t pml4_offset = (virtual_address >> 39) & 0x1FF;
    uint16_t pdpt_offset = (virtual_address >> 30) & 0x1FF;
    uint16_t pd_offset = (virtual_address >> 21) & 0x1FF;
    uint16_t pt_offset = (virtual_address >> 12) & 0x1FF;

    uint64_t *pdpt, *pd, *pt;
    if (!(pml4[pml4_offset] & PAGE_PRESENT)) return false;
    pdpt = (uint64_t *)(pml4[pml4_offset] & ~0xFFF);
    if (!(pdpt[pdpt_offset] & PAGE_PRESENT)) return false;
    pd = (uint64_t *)(pdpt[pdpt_offset] & ~0xFFF);
    if (!(pd[pd_offset] & PAGE_PRESENT)) return false;
    pt = (uint64_t *)(pd[pd_offset] & ~0xFFF);
    pt[pt_offset] = 0;

    if (check_page_is_empty(pt)) {
        pd[pd_offset] = 0;
    }
    if (check_page_is_empty(pd)) {
        pdpt[pdpt_offset] = 0;
    }
    if (check_page_is_empty(pdpt)) {
        pml4[pml4_offset] = 0;
    }

    invalidate_tlb(virtual_address);
}

// Refer to 5.3.3 of AMD manual
// ! IMPORTANT PHYSICAL ADDRESS MUST BE 4096 ALIGNED
// ! (well, not actually that important since this code clears the lower 12 bits
// anyways...) ! make sure flags include PAGE_PRESENT
void map_page(uint64_t *pml4, uint64_t virtual_address,
              uint64_t physical_address, uint16_t flags) {
    // each offset is 9 bits
    uint16_t pml4_offset = (virtual_address >> 39) & 0x1FF;
    uint16_t pdpt_offset = (virtual_address >> 30) & 0x1FF;
    uint16_t pd_offset = (virtual_address >> 21) & 0x1FF;
    uint16_t pt_offset = (virtual_address >> 12) & 0x1FF;

    // physical offset covers the 12 bits (offset from the 4096 aligned address)
    // uint16_t physical_offset = virtual_address & 0x7FF;

    // tables

    // we do address & (~0xFFF) because first 12 bits are flags
    // refer to Figure 5-19 (5.3.3) in amd manual as example
    uint64_t *pdpt, *pd, *pt;
    if (!(pml4[pml4_offset] & PAGE_PRESENT)) {
        pdpt = allocate_page();
        // todo: do something if null is returned
        clear_page((uint8_t *)pdpt);

        // must & ~0xfff usually first 12 bits are zero
        // because the tables are 4096 aligned (we might not need it?)
        pml4[pml4_offset] = ((uint64_t)pdpt & ~0xFFF) | flags;
    } else {
        pdpt = (uint64_t *)(pml4[pml4_offset] & ~0xFFF);
    }

    if (!(pdpt[pdpt_offset] & PAGE_PRESENT)) {
        pd = allocate_page();
        clear_page((uint8_t *)pd);
        pdpt[pdpt_offset] = ((uint64_t)pd & ~0xFFF) | flags;
    } else {
        pd = (uint64_t *)(pdpt[pdpt_offset] & ~0xFFF);
    }

    if (!(pd[pd_offset] & PAGE_PRESENT)) {
        pt = allocate_page();
        clear_page((uint8_t *)pt);
        pd[pd_offset] = ((uint64_t)pt & ~0xFFF) | flags;
    } else {
        pt = (uint64_t *)(pd[pd_offset] & ~0xFFF);
    }

    pt[pt_offset] = (physical_address & ~0xFFF) | flags;
    invalidate_tlb(virtual_address);
}

bool verify_mapping(uint64_t *pml4, uint64_t virtual_address) {
    uint16_t pml4_offset = (virtual_address >> 39) & 0x1FF;
    uint16_t pdpt_offset = (virtual_address >> 30) & 0x1FF;
    uint16_t pd_offset = (virtual_address >> 21) & 0x1FF;
    uint16_t pt_offset = (virtual_address >> 12) & 0x1FF;

    uint64_t *pdpt, *pd, *pt;

    if (!(pml4[pml4_offset] & PAGE_PRESENT)) {
        return false;
    }

    pdpt = (uint64_t *)(pml4[pml4_offset] & ~0xFFF);

    if (!(pdpt[pdpt_offset] & PAGE_PRESENT)) {
        return false;
    }

    pd = (uint64_t *)(pdpt[pdpt_offset] & ~0xFFF);

    if (!(pd[pd_offset] & PAGE_PRESENT)) {
        return false;
    }

    pt = (uint64_t *)(pd[pd_offset] & ~0xFFF);

    if (!(pt[pt_offset] & PAGE_PRESENT)) {
        return false;
    }

    return true;
}
