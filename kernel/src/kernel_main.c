#include <stdint.h>

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 *
 * This **must** be the first symbol of the kernel code output file, at 0x2000.
 */
void _Noreturn kernel_main(void) {
    volatile uint16_t *vga = (volatile uint16_t *) 0xB8000;
    unsigned i = 0;

    while (1) {
        vga[0] = ('K' + i) | 0x0f00;
        vga[1] = ('e' - i) | 0x0f00;
        vga[2] = ('r' - i) | 0x0f00;
        vga[3] = ('n' - i) | 0x0f00;
        vga[4] = ('e' - i) | 0x0f00;
        vga[5] = ('l' - i) | 0x0f00;

        i = 0x20 - i;

        // busy sleep loop
        for (unsigned s = 0; s != 100000000; s++) {
            asm volatile(""::);
        }
    }
}
