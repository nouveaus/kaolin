#include "io.h"

#include "vga.h"
#include "va_list.h"

void puti(int num) {
    // uses only one branch if 0
    if (num == 0) {
        vga_putchar('0');
        return;
    }

    if (num < 0) vga_putchar('-');
    
    // this finds the biggest divisor for the number
    int div = 1;
    while (num / div >= 10) div *= 10;
    
    while (div > 0) {
        vga_putchar((num / div) + '0');
        num %= div;
        div /= 10;
    }
}

void putc(char c) { vga_putchar(c); }

void puts(char *s) { vga_write_string(s); }

void krintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char c;
    while (c = *format++) {
        if (c == '%') {
            c = *format++;
            switch (c) {
                case '%': {
                    putc('%');
                    break;
                }
                case 'c': {
                    char value = va_arg(args, int);
                    putc(value);
                    break;
                }
                case 's': {
                    char *value = va_arg(args, char *);
                    puts(value);
                    break;
                }
                case 'd' : {
                    int value = va_arg(args, int);
                    puti(value);
                }
                // TODO: add float support
            }
        } else {
            vga_putchar(c);
        }
    }

    va_end(args);
}