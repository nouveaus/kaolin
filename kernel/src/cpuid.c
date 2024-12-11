#include <stdint.h>

#include "io.h"
#include "cpuid.h"

void print_reg(uint32_t ebx) {
    char register_val[5] = {};

    for (int i = 3; i >= 0; i--) {
        register_val[3 - i] = ((ebx << CHAR_BIT * i) >> (CHAR_BIT * 3));
    }
    register_val[4] = 0;

    for (int i = 0; i < 4; i++) {
        krintf("%d:", i);
        krintf("%c\n", register_val[i]);
    }
}

void print_vendor(uint32_t ebx, uint32_t ecx, uint32_t edx) {
    print_reg(ebx);
    print_reg(ecx);
    print_reg(edx);
}

void call_cpuid(int eax) {
    uint32_t ebx, ecx, edx;
    ebx = 0;
    ecx = 0;
    edx = 0;

    asm (
        "mov %3, %%eax;"
        "cpuid;"
        : "=b" (ebx), "=d" (edx), "=c" (ecx)
        : "r" (eax)
    );

    print_vendor(ebx, ecx, edx);
}
