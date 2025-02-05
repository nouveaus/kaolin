#include <stdbool.h>

#include "../memory/paging.h"
#include "klib.h"

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
};

// Way it works:
// heap_block | actual memory
// We allocate a extra 16 bytes for this metadata
// We always return the memory address 16 bytes after

static void *heap_start = NULL;
static void *heap_end = NULL;

static void *get_heap_address(struct heap_block *block);
static struct heap_block *allocate_free_block(void *base, size_t size,
                                              struct heap_block *next,
                                              struct heap_block *back);

static void *get_heap_address(struct heap_block *block) {
    return (void *) (block + 1);
}

static struct heap_block *allocate_free_block(void *base, size_t size,
                                              struct heap_block *next,
                                              struct heap_block *back) {
    struct heap_block *new = (struct heap_block *) (base);
    new->size = (size - sizeof(struct heap_block));
    new->free = true;
    new->next = next;
    new->back = back;
    return new;
}

void heap_init(void) {
    // todo: check if page cache disable is needed
    heap_start = kmap_page((void *) HEAP_START, PAGE_PRESENT | PAGE_RW | PAGE_CACHE_DISABLE);
    heap_end = (void *) ((uint64_t) heap_start + HEAP_INITIAL_SIZE);

    allocate_free_block(heap_start, HEAP_INITIAL_SIZE, NULL, NULL);
    //  for expand heap
}

void *kmalloc(size_t size) {
    size = (size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);

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
                        (uint8_t *) block + sizeof(struct heap_block) + size,
                        free_size, block->next, block);
                if (block == heap_end) heap_end = new;
                block->next = new;
            }
            return get_heap_address(block);
        }
        before = block;
        block = block->next;
    }

    // we need to allocate more pages

    if ((uint8_t *) heap_end + size + sizeof(struct heap_block) >
        (uint8_t *) (KERNEL_MAPPING_ADDRESS | HEAP_MAX_SIZE)) {
        return NULL;
    }

    void *new_address =
            mmap((void *) (KERNEL_MAPPING_ADDRESS ^ (uint64_t) heap_end),
                 size + sizeof(struct heap_block), &heap_end);
    new = allocate_free_block(new_address, size, NULL, before);
    before->next = new;

    return get_heap_address(new);
}

void free(void *address) {
    if (address == NULL) return;
    struct heap_block *block =
            (struct heap_block *) ((uint8_t *) address - sizeof(struct heap_block));
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
