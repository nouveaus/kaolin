#include "apic.h"

#include "cpuid.h"
#include "msr.h"
#include "serial.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

bool apic_is_supported(void) {
    uint32_t eax, ebx, ecx, edx;
    call_cpuid(1, &eax, &ebx, &ecx, &edx);
    return (edx & CPUID_FEAT_EDX_APIC) != 0;
}

int apic_get_id(void) {
    uint32_t eax, ebx, ecx, edx;
    call_cpuid(1, &eax, &ebx, &ecx, &edx);
    return (ebx >> 24) & 0xFF;
}

static void cpu_set_apic_base(uintptr_t apic) {
    uint32_t edx = 0;
    // osdev says 0xfffff0000 but thats 36 bits - might be a typo?
    uint32_t eax = (apic & 0xFFFFFF000) | IA32_APIC_BASE_MSR_ENABLE;
    
    #ifdef __PHYSICAL_MEMORY_EXTENSION__
        edx = (apic >> 32) & 0x0F;
    #endif

    cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

static uintptr_t cpu_get_apic_base(void) {
    uint32_t eax, edx;
    cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);

    #ifdef __PHYSICAL_MEMORY_EXTENSION__
        return (eax & 0xFFFFF000) | ((edx & 0x0F) << 32);
    #else
        return (eax & 0xFFFFF000);
    #endif
}

// todo: move to own file later on

// todo: read from acpi to figure out ioapicbase
// todo: since its not guaranteed to be same in all systems

#define BASE_LAPIC 0xFEE00000

void write_reg(void *const lapicbase, const uint32_t reg, const uint32_t value) {
    uint32_t volatile *const lapic = (uint32_t volatile *const)lapicbase;
    lapic[reg] = value;
}

uint32_t read_reg(void *const lapicbase, const uint32_t reg) {
    uint32_t volatile *const lapic = (uint32_t volatile *const)lapicbase;
    return lapic[reg];
}

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_DATA	(PIC1+1)
#define PIC2_DATA	(PIC2+1)


void enable_apic(void) {
    // disable the PIC (important! according to osdev)
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    cpu_set_apic_base(cpu_get_apic_base());
    write_reg((void*)BASE_LAPIC, 0xF0, read_reg((void*)BASE_LAPIC, 0xF0) | 0x100);
}

