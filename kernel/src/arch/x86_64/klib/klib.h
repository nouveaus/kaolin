#pragma once

#include <stddef.h>
#include <stdint.h>

// Sleep for given ticks
void ksleep(int ticks);

struct interrupt_frame;

// Trap gate
void trap(struct interrupt_frame *frame);

// Initialises the heap by mapping it in virtual memory.
void heap_init(void);
// Allocates memory from a specified size.
void *kmalloc(size_t size);
// Frees the memory allocated from heap.
void free(void *address);
