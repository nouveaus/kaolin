#include "vga.h"
#include "cpuid.h"
#include "apic.h"
#include "io.h"

void _Noreturn kernel_main(void) __attribute__((section(".text.kernel_main")));

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 */
void _Noreturn kernel_main(void) {
    vga_initialize();
    if (!cpuid_is_supported()) {
        puts("cpuid not supported\n");
        asm volatile("hlt");
    }

    uint32_t eax, ebx, ecx, edx;
    // get vendor string
    call_cpuid(0, &eax, &ebx, &ecx, &edx);
    print_vendor(ebx, ecx, edx);

    if (!apic_is_supported()) {
        puts("APIC not supported\n");
        asm volatile("hlt");
    }

    enable_apic();
    puts("APIC enabled\n");

    while (1) {
        // busy sleep loop
        for (unsigned s = 0; s != 100000000; s++) {
            asm volatile(""::);
        }
    }
}
