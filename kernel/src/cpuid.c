#include <stdint.h>

#include "io.h"
#include "cpuid.h"

#define CHAR_BIT 8

// Prints the value of the register
static void print_reg(const uint32_t r) {
    char register_val[5] = {};

    // stores each byte from the register in reverse
    for (int i = 3; i >= 0; i--) {
        register_val[3 - i] = (r >> (CHAR_BIT * (3 - i))) & 0xFF;
    }
    register_val[4] = 0;

    puts(register_val);
}

void print_vendor(const uint32_t ebx, const uint32_t ecx, const uint32_t edx) {
    print_reg(ebx);
    print_reg(edx);
    print_reg(ecx);
    putc('\n');
}

void call_cpuid(const int code, uint32_t *const eax, uint32_t * const ebx, uint32_t *const ecx, uint32_t *const edx) {
    asm (
        "mov %3, %%eax\n"
        "cpuid\n"
        : "=b" (*ebx), "=d" (*edx), "=c" (*ecx), "=a" (*eax)
        : "r" (code)
    );
}
