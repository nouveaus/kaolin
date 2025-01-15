#pragma once

#include <stdint.h>
#include <stddef.h>

// Initialises the heap by mapping it in virtual memory.
void heap_init(uint64_t *pml4);
// Allocates memory from a specified size.
void *kmalloc(size_t size);
// Frees the memory allocated from heap.
void free(void *address);
