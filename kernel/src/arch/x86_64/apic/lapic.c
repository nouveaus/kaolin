#include "lapic.h"

#include "../cpu/cpuid.h"
#include "../cpu/msr.h"
#include "../serial/serial.h"

#define IA32_APIC_BASE_MSR        0x1B
#define IA32_APIC_BASE_MSR_BSP    0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

bool apic_is_supported(void) {
    uint32_t eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    return (edx & CPUID_FEAT_EDX_APIC) != 0;
}

int apic_get_id(void) {
    uint32_t eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    return (ebx >> 24) & 0xFF;
}

static void cpu_set_apic_base(uintptr_t apic) {
    uint32_t edx = 0;
    // osdev says 0xfffff0000 but thats 36 bits - might be a typo?
    uint32_t eax = (apic & 0xFFFFF000) | IA32_APIC_BASE_MSR_ENABLE;

    cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

static uintptr_t cpu_get_apic_base(void) {
    uint32_t eax, edx;
    cpu_get_msr(IA32_APIC_BASE_MSR, &eax, &edx);

    return (eax & 0xFFFFF000);
}

// todo: read from acpi to figure out ioapicbase
// todo: since its not guaranteed to be same in all systems

void write_reg(void *lapicbase, uint32_t reg, uint32_t value) {
    uint32_t volatile *lapic = (uint32_t volatile *) lapicbase;
    lapic[reg] = value;
}

uint32_t read_reg(void *lapicbase, uint32_t reg) {
    uint32_t volatile *lapic = (uint32_t volatile *) lapicbase;
    return lapic[reg];
}

#define PIC1      0x20 /* IO base address for master PIC */
#define PIC2      0xA0 /* IO base address for slave PIC */
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)

bool map_apic(void) {
    map_page(LAPIC_VIRTUAL_ADDRESS, BASE_LAPIC,
             PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE);
    return verify_mapping(LAPIC_VIRTUAL_ADDRESS);
}

void enable_apic(void) {
    // disable the PIC (important! according to osdev)
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    cpu_set_apic_base(cpu_get_apic_base());
    // ! triple faults here
    write_reg((void *) LAPIC_VIRTUAL_ADDRESS, 0xF0,
              read_reg((void *) LAPIC_VIRTUAL_ADDRESS, 0xF0) | 0x100);
}

void send_apic_eoi(void) {
    *(uint32_t volatile *) (LAPIC_VIRTUAL_ADDRESS + 0xB0) = 0;
}
