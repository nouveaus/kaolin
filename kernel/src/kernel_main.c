#include "vga.h"
#include "io.h"

void _Noreturn kernel_main(void) __attribute__((section(".text.kernel_main")));

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 */
void _Noreturn kernel_main(void) {
    vga_initialize();

    char message[] = "X Hello world!\n";
    unsigned int i = 0;

    while (1) {
        message[0] = '0' + i;
        i = (i + 1) % 10;

        //vga_write_string(message);
        krintf("%sThe number is: %d, float is: %f\n", message, 5, 3.9999);
        vga_set_color(1 + (i % 6), VGA_COLOR_BLACK);
        char c = read_char();
        krintf("char read: %c\n", c);
        char s[15];
        read_string(s);
        krintf("string read: %s\n", s);


        // busy sleep loop
        for (unsigned s = 0; s != 100000000; s++) {
            asm volatile(""::);
        }
    }
}
