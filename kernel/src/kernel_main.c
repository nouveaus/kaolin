#include "vga.h"
#include "cpuid.h"

void _Noreturn kernel_main(void) __attribute__((section(".text.kernel_main")));

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 */
void _Noreturn kernel_main(void) {
    vga_initialize();
    vga_write_string(cpuid_is_supported() ? "cpuid supported!\n" : "cpuid not supported\n");

    while (1) {
        // busy sleep loop
        for (unsigned s = 0; s != 100000000; s++) {
            asm volatile(""::);
        }
    }
}
