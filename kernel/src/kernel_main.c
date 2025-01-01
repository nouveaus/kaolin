#include "vga.h"
#include "cpuid.h"
#include "apic.h"
#include "ioapic.h"
#include "io.h"
#include "acpi.h"
#include "msr.h"

#include <stdint.h>

void _Noreturn kernel_main(void) __attribute__((section(".text.kernel_main")));

static inline void read_acpi(void);

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 */
void _Noreturn kernel_main(void) {
    vga_initialize();

    uint32_t eax, ebx, ecx, edx;
    // get vendor string
    call_cpuid(0, &eax, &ebx, &ecx, &edx);
    print_vendor(ebx, ecx, edx);

    if (!apic_is_supported()) {
        puts("APIC not supported\n");
        asm volatile("hlt");
    }

    if (!cpu_has_msr()) {
        puts("MSR not supported\n");
        asm volatile("hlt");
    }

    read_acpi();

    enable_apic();

    // todo: load idt here

    int apic_id = apic_get_id();
    krintf("APIC ID: %d\n", apic_id);

    // we have interrupt after 31 since 0-31 are reserved for errors
    ioapic_set_redirect((uintptr_t*)IOAPICBASE, 0x20, apic_id);


    char message[] = "X Hello world!\n";
    unsigned int i = 0;

    while (1) {
        message[0] = '0' + i;
        i = (i + 1) % 10;

        //vga_write_string(message);
        krintf("%sThe number is: %d, float is: %f\n", message, 5, 3.9999);
        vga_set_color(1 + (i % 6), VGA_COLOR_BLACK);

        // busy sleep loop
        for (unsigned s = 0; s != 500000000; s++) {
            asm volatile(""::);
        }
    }
}

static inline void read_acpi(void) {
    if (!rsdp_find()) {
        puts("Could not find RSDP\n");
        asm volatile("hlt");
    }

    puts("Found RSDP\nRSDP signature:");
    rsdp_print_signature();
    krintf("RSDP revision: %d\n", rsdp_get_revision());
    if (!rsdp_verify()) {
        krintf("Could not verify RSDP\n");
        asm volatile("hlt");
    }
    puts("Verified RSDP\n");
    
    if (!rsdt_verify()) {
        puts("Could not verify RSDT\n");
    }
    puts("Verified RSRT\n");

    if (!madt_find()) {
        puts("Could not find MADT\n");
        asm volatile("hlt");
    }

    puts("Found MADT\n");

    if (!madt_verify()) {
        puts("Could not verify MADT\n");
        asm volatile("hlt");
    }
    puts("Verified MADT\n");

    krintf("LAPIC address: %x\n", madt_get_lapic_address());

    size_t count = ioapic_get_entry_count();
    krintf("Detected %d ioapic on device\n", count);

    krintf("Address is %x\n", get_first_ioapic_address());
}