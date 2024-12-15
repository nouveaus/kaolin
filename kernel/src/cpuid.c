#include <stdint.h>

#include "io.h"
#include "cpuid.h"

bool cpuid_is_supported(void) {
    // Source: https://wiki.osdev.org/CPUID
    // this inverts the id bit in the stored
    // eflags and compares it
    bool supported;
    asm (
        "pushfl\n"
        "pushfl\n"
        "popl %%eax\n"
        // invert ID bit
        "xorl $0x00200000, %%eax\n"
        "pushl %%eax\n"
        "popfl\n"
        
        // verify change here
        "pushfl\n"
        "popl %%eax\n"
        "popfl\n"
        "andl $0x00200000, %%eax\n"
        "setnz %0\n"
        : "=r" (supported)
        :
        : "eax", "cc"
    );
    return supported;
}

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
