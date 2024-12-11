#include "io.h"

#include "vga.h"
#include "serial.h"

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

static uint8_t ps2_controller_read(void) {
    while (!(inb(STATUS_PORT) & 0x01));
    return inb(DATA_PORT);
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

static char scan_code_to_ascii(const uint8_t value) {
    // static so we dont need to initialise it all the time
    // Uses US qwerty, table derived from: https://wiki.osdev.org/PS/2_Keyboard
    static const char lookup_table[] = {
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

// todo: for some reason it reads in garbage values
void read_string(char *s) {
    char c;
    while ((c = read_char()) != '\n') {
        *s = c;
        s++;
    }
    *s = 0;
}
