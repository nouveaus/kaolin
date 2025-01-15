#include "heap.h"

#include <stdbool.h>

#include "../../../io.h"
#include "paging.h"

#define HEAP_START 0x01000000

// needs to be <= 4096 bytes
#define HEAP_INITIAL_SIZE 0x01000

// arbitrary number
#define HEAP_MAX_SIZE 0x10000000

struct heap_block {
    uint64_t size;
    struct heap_block *next;
    struct heap_block *back;
    bool free;
} __attribute__((aligned(16)));

// Way it works:
// heap_block | actual memory
// We allocate a extra 16 bytes for this metadata
// We always return the memory address 16 bytes after

static void *heap_start = (void *)(KERNEL_MAPPING_ADDRESS | HEAP_START);
static void *heap_end =
    (void *)((KERNEL_MAPPING_ADDRESS | HEAP_START) + HEAP_INITIAL_SIZE);
static uint64_t *pages_pml4;

static void *heap_expand(size_t size);
static void *get_heap_address(struct heap_block *block);
static struct heap_block *allocate_free_block(void *base, size_t size,
                                              struct heap_block *next,
                                              struct heap_block *back);

static void *get_heap_address(struct heap_block *block) {
    return (void *)(block + 1);
}

static struct heap_block *allocate_free_block(void *base, size_t size,
                                              struct heap_block *next,
                                              struct heap_block *back) {
    struct heap_block *new = (struct heap_block *)(base);
    new->size = (size - sizeof(struct heap_block));
    new->free = true;
    new->next = next;
    new->back = back;
    return new;
}

void heap_init(uint64_t *pml4) {
    map_page(pml4, KERNEL_MAPPING_ADDRESS | HEAP_START, (uint64_t)HEAP_START,
             PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE);
    //krintf("v: %d\n",
    //       verify_mapping(pml4, KERNEL_MAPPING_ADDRESS | HEAP_START));
     allocate_free_block(heap_start, HEAP_INITIAL_SIZE,
                         NULL, NULL);
    //  for expand heap
    pages_pml4 = pml4;
}

// todo: move this into klib
void *kmalloc(size_t size) {
    // round up to nearest 16 divisible
    size = (size + 15) & ~15;

    struct heap_block *before = NULL;
    struct heap_block *block = heap_start;
    struct heap_block *new;
    while (block != NULL) {
        if (block->free && size <= block->size) {
            // adjust size
            size_t free_size = block->size - size;
            block->size = size;
            block->free = false;

            // make new free block
            if (free_size > sizeof(struct heap_block)) {
                new = allocate_free_block(
                    (uint8_t *)block + sizeof(struct heap_block) + size, free_size,
                    block->next, block);
                if (block == heap_end) heap_end = new;
                block->next = new;
            }
            return get_heap_address(block);
        }
        before = block;
        block = block->next;
    }

    // we need to allocate more pages
    void *new_address = heap_expand(size + sizeof(struct heap_block));
    if (new_address == NULL) return new_address;
    new = allocate_free_block(new_address, size, NULL, before);
    before->next = new;
    
    return get_heap_address(new);
}

void free(void *address) {
    if (address == NULL) return;
    struct heap_block *block =
        (struct heap_block *)((uint8_t *)address - sizeof(struct heap_block));
    block->free = true;

    if (block->next && block->next->free) {
        block->size += block->next->size;
        block->next = block->next->next;
    }

    if (block->back && block->back->free) {
        block->back->size += block->size;
        block->back->next = block->next;
    }
}

static void *heap_expand(size_t size) {
    if ((uint8_t *)heap_end + size >
        (uint8_t *)(KERNEL_MAPPING_ADDRESS | HEAP_MAX_SIZE)) {
        return NULL;
    }

    // divide by 0x1000 for 4096 bytes (each page takes up that)
    // add by 0xFFF to ensure 4096 alignment
    size_t pages_required = (size + 0xFFF) / 0x1000;
    void *block_start = heap_end;
    for (size_t i = 0; i < pages_required; i++) {
        // xor to get the physical address
        map_page(pages_pml4, (uint64_t)heap_end,
                 KERNEL_MAPPING_ADDRESS ^ (uint64_t)heap_end,
                 PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE);
        heap_end = (uint8_t *)heap_end + 0x1000;
    }

    return block_start;
}
