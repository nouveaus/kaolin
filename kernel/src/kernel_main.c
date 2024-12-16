#include "vga.h"
#include "cpuid.h"
#include "io.h"

void _Noreturn kernel_main(void) __attribute__((section(".text.kernel_main")));

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

    char message[] = "X Hello world!\n";
    unsigned int i = 0;

    while (1) {
        message[0] = '0' + i;
        i = (i + 1) % 10;

        //vga_write_string(message);
        krintf("%sThe number is: %d, float is: %f\n", message, 5, 3.9999);
        vga_set_color(1 + (i % 6), VGA_COLOR_BLACK);

        // busy sleep loop
        for (unsigned s = 0; s != 100000000; s++) {
            asm volatile(""::);
        }
    }
}
