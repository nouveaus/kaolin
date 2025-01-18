#pragma once

#include <stddef.h>
#include <stdint.h>

// Sleep for given ticks
void ksleep(int ticks);

struct interrupt_frame {
    // --- 1) General-purpose registers pushed by our assembly stub ---
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    // Interrupt/exception number (pushed by the stub)
    uint64_t int_no;

    // Error code:
    //  - Automatically pushed by CPU for some exceptions (e.g., page fault),
    //    or
    //  - Pushed as 0 by our assembly stub for exceptions that do not push an error code
    uint64_t err_code;

    // --- 2) IRET frame (pushed automatically by CPU) ---
    // RIP (instruction pointer) at time of interrupt/exception
    uint64_t rip;
    // Code segment selector at time of interrupt/exception
    uint64_t cs;
    // Saved RFLAGS
    uint64_t rflags;
    // Only pushed by CPU if privilege-level change (e.g., user->kernel)
    uint64_t rsp;
    // Only pushed by CPU if privilege-level change
    uint64_t ss;
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