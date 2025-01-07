#include "io.h"

#include <stdarg.h>
#include <stdint.h>

void puti(int num) {
    // uses only one branch if 0
    if (num == 0) {
        putc('0');
        return;
    }

    if (num < 0) {
        putc('-');
        num = -num;
    }

    // this finds the biggest divisor for the number
    int div = 1;
    while (num / div >= 10) div *= 10;

    while (div > 0) {
        putc((num / div) + '0');
        num %= div;
        div /= 10;
    }
}

void putd(double f) {
    if (f < 0.0) {
        putc('-');
        f = -f;
    }

    // + 0.5 for rounding
    int int_part = f;
    const int scale = 10000000;
    int frac = (f - (int) f) * scale + 0.5;

    if (frac == scale) {
        frac = 0;
        int_part++;
    }

    puti(int_part);
    putc('.');
    // we lose precision if we use puti here
    putc('0' + ((frac) / 1000000) % 10);
    putc('0' + ((frac) / 100000) % 10);
    putc('0' + ((frac) / 10000) % 10);
    putc('0' + ((frac) / 1000) % 10);
    putc('0' + ((frac) / 100) % 10);
    putc('0' + ((frac) / 10) % 10);
    putc('0' + ((frac) % 10));
}

static void hex(uint32_t num) {
    puts("0x");

    for (int i = sizeof(num) * 8 - 4; i >= 0; i -= 4) {
        putc("0123456789ABCDEF"[(num >> i) & 0xF]);
    }
}

void krintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (char c; (c = *format++);) {
        if (c != '%') {
            putc(c);
            continue;
        }

        switch ((c = *format++)) {
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
                const char *value = va_arg(args, const char *);
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
                putd(value);
                break;
            }
            case 'x': {
                uint32_t value = va_arg(args, uint32_t);
                hex(value);
                break;
            }
            default: {
                putc('%');
                putc(c);
                break;
            }
        }
    }

    va_end(args);
}
