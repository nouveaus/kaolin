#include "cpuid.h"
#include "msr.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

bool apic_is_supported(void) {
    uint32_t eax, ebx, ecx, edx;
    call_cpuid(1, &eax, &ebx, &ecx, &edx);
    return edx & CPUID_FEAT_EDX_APIC != 0;
}

static void cpu_set_apic_base(uintptr_t apic) {
    uint32_t edx = 0;
    // osdev says 0xfffff0000 but thats 36 bits - might be a typo?
    uint32_t eax = (apic & 0xfffff000) | IA32_APIC_BASE_MSR_ENABLE;
    
    #ifdef __PHYSICAL_MEMORY_EXTENSION__
        edx = (apic >> 32) & 0x0f;
    #endif

    cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

static uintptr_t cpu_get_apic_base(void) {
    uint32_t eax, edx;
    cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);

    #ifdef __PHYSICAL_MEMORY_EXTENSION__
        return (eax & 0xfffff000) | ((edx & 0x0f) << 32);
    #else
        return (eax & 0xfffff000);
    #endif
}

// todo: move to own file later on

static uint32_t read_reg(uint32_t offset) {
    volatile uintptr_t *reg = (volatile uintptr_t *)(IA32_APIC_BASE_MSR + offset);
    return *reg;
}

static void write_reg(uint32_t offset, uint32_t value) {
    volatile uintptr_t *reg = (volatile uintptr_t *)(IA32_APIC_BASE_MSR + offset);
    *reg = value;
}

void enable_apic(void) {
    cpu_set_apic_base(cpu_get_apic_base());
    write_reg(0xF0, read_reg(0xF0) | 0x100);
}