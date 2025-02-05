#include "msr.h"

#include <stdbool.h>
#include <stdint.h>

#include "cpuid.h"

bool cpu_has_msr(void) {
    uint32_t eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    return (edx & CPUID_FEAT_EDX_MSR) != 0;
}

void cpu_get_msr(uint32_t msr, uint32_t *lo, uint32_t *hi) {
    asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void cpu_set_msr(uint32_t msr, uint32_t lo, uint32_t hi) {
    asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}
