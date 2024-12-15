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