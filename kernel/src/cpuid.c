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

void print_reg(uint32_t r) {
    char register_val[5] = {};

    // stores each byte from the register in reverse
    for (int i = 3; i >= 0; i--) {
        register_val[3 - i] = (r >> (CHAR_BIT * (3 - i))) & 0xFF;
    }
    register_val[4] = 0;

    puts(register_val);
}

void print_vendor(uint32_t ebx, uint32_t ecx, uint32_t edx) {
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

bool apic_is_supported(void) {
    uint32_t eax, ebx, ecx, edx;
    call_cpuid(1, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_FEAT_EDX_APIC != 0;
}
