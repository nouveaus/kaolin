#include <stdint.h>

#include "vga.h"

void _Noreturn kernel_main(void) __attribute__((section(".text.kernel_main")));

static void print_string(unsigned index, uint16_t mask, const char *message);

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 */
void _Noreturn kernel_main(void) {
    // print_string(100, 0x0e00, "data segment works now btw");
    // print_string(200, 0x0900, "another one");

    // volatile uint16_t *vga = (volatile uint16_t *) 0xB8000;
    // unsigned i = 0;

    terminal_initialize();

    unsigned color = VGA_COLOR_BLUE;

    while (1) {
        // vga[0] = ('K' + i) | 0x0f00;
        // vga[1] = ('e' - i) | 0x0f00;
        // vga[2] = ('r' - i) | 0x0f00;
        // vga[3] = ('n' - i) | 0x0f00;
        // vga[4] = ('e' - i) | 0x0f00;
        // vga[5] = ('l' - i) | 0x0f00;

        // i = 0x20 - i;

        terminal_writestring("Hello world!\n");
        terminal_setcolor(color);
        color = color == VGA_COLOR_BLUE ? VGA_COLOR_GREEN : VGA_COLOR_BLUE;

        // busy sleep loop
        for (unsigned s = 0; s != 100000000; s++) {
            asm volatile(""::);
        }
    }
}

void print_string(unsigned index, uint16_t mask, const char *message) {
    volatile uint16_t *vga = (volatile uint16_t *) 0xB8000;
    char c;

    for (unsigned i = 0; (c = message[i]); i++) {
        vga[index + i] = c | mask;
    }
}
