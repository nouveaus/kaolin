#include "io.h"

#include "vga.h"
#include "va_list.h"

#include <stdint.h>

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

static void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#define DATA_PORT 0x60
#define STATUS_PORT 0x64

static uint8_t ps2_controller_read(void) {
    while (!(inb(STATUS_PORT) & 0x01));
    return inb(STATUS_PORT);
}

static void ps2_controller_write(uint8_t value) {
    while (inb(STATUS_PORT) & 0x02);
    outb(DATA_PORT, value);
}

static void ps2_controller_clear_output_buffer(void) {
    while (inb(STATUS_PORT) & 0x01) {
        inb(DATA_PORT);
    }
}

// for now we support only US qwerty keyboard
// god bless america

static char scan_code_to_ascii(uint8_t value) {
    // static so we dont need to initialise it all the time
    static char lookup_table[] = {
        0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, '\t',
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0,
    };

    if (value < sizeof(lookup_table)) return lookup_table[value];
    return value;
}

char read_char(void) {
    ps2_controller_clear_output_buffer();
    return scan_code_to_ascii(ps2_controller_read());
}

