#pragma once

#include <stddef.h>
#include <stdint.h>

// Sleep for given ticks
void ksleep(int ticks);

struct interrupt_frame {
    uint64_t one;
    uint64_t two;
    uint64_t three;
    uint64_t four;
    uint64_t five;
    uint64_t six;
} __attribute__((packed));

// Trap gate
void trap(struct interrupt_frame *frame);

// Initialises the heap by mapping it in virtual memory.
void heap_init(void);
// Allocates memory from a specified size.
void *kmalloc(size_t size);
// Frees the memory allocated from heap.
void free(void *address);

void exception_handler(struct interrupt_frame *frame);
