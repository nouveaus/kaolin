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
__attribute__((interrupt)) void trap(struct interrupt_frame *frame);

// Initialises the heap by mapping it in virtual memory.
void heap_init(void);
// Allocates memory from a specified size.
void *kmalloc(size_t size);
// Frees the memory allocated from heap.
void free(void *address);

// Compare each byte of the two addresses given the count
int memcmp(const void *a, const void *b, size_t count);

// Sets each byte in destination given the value and size
void memset(void *dst, char val, size_t size);

// Copies the memory from source to destination given the length
void *memcpy(void *dst, const void *src, size_t len);

// Moves the memory from source to destination given the length
// (Safe for overlapping)
void *memmove(void *dst, const void *src, size_t len);

// Exception Handler
__attribute__((interrupt)) void exception_handler(struct interrupt_frame *frame);

// Stops the kernel
void _Noreturn _die(void);
