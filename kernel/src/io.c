#include "io.h"

#include "vga.h"

#include <stdint.h>
#include <stdarg.h>

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


#define PRINT_7_FRAC_DIGITS(val)            \
    do {                                    \
        putc('0' + ((val) / 1000000) % 10); \
        putc('0' + ((val) / 100000) % 10);  \
        putc('0' + ((val) / 10000) % 10);   \
        putc('0' + ((val) / 1000) % 10);    \
        putc('0' + ((val) / 100) % 10);     \
        putc('0' + ((val) / 10) % 10);      \
        putc('0' + ((val) % 10));           \
    } while (0)

void putf(double f) {
    if (f < 0.0) {
        putc('-');
        f = -f;
    }

    // + 0.5 for rounding
    int int_part = f;
    const int scale = 10000000;
    int frac = (f - (int)f) * scale + 0.5;
    
    if (frac == scale) {
        frac = 0;
        int_part++;
    }
    
    puti(int_part);
    putc('.');
    // we lose precision if we use puti here
    PRINT_7_FRAC_DIGITS(frac);
}

void putc(const char c) { 
    vga_putchar(c);
}

void puts(const char *s) {
    vga_write_string(s);
}

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
                    break;
                }
                case 'f': {
                    double value = va_arg(args, double);
                    putf(value);
                    break;
                }
                // TODO: add float support
            }
        } else {
            vga_putchar(c);
        }
    }

    va_end(args);
}
