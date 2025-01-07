#include "../../../io.h"


#include "../vga/vga.h"

void putc(char c) {
    vga_putchar(c);
}

void puts(const char *s) {
    vga_write_string(s);
}
