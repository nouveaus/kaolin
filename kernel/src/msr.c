#include "msr.h"

#include "cpuid.h"
#include <stdint.h>
#include <stdbool.h>

#define CPUID_FLAG_MSR 1 << 5;

bool cpu_has_msr(void) {
    uint32_t eax, ebx, ecx, edx;
    call_cpuid(1, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_FLAG_MSR;
}  

void cpu_get_msr(uint32_t msr, uint32_t *const lo, uint32_t *const hi) {
    asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void cpu_set_msr(uint32_t msr, const uint32_t lo, const uint32_t hi) {
    asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}
