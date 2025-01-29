#include "vga.h"

#include "../serial/serial.h"

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static volatile uint16_t *const VGA_BUFFER = (volatile uint16_t *) 0xB8000;

static size_t terminal_row = 0;
static size_t terminal_column = 0;
static uint8_t terminal_color;

static uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) (color << 8) | (uint16_t) c;
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void vga_initialize(void) {
    vga_set_color(VGA_COLOR_BROWN, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
}

void vga_set_color(enum vga_color fg, enum vga_color bg) {
    terminal_color = (bg << 4) | fg;
}

void vga_scroll(int lines) {
    // move the [HEIGHT - lines, HEIGHT) buffer to [0, HEIGHT - lines), then clear [lines, HEIGHT)

    // todo: replace with memmove
    //  memmove(dest, src, (VGA_HEIGHT - lines) * VGA_WIDTH * sizeof(*src));

    volatile uint16_t *dest = VGA_BUFFER;
    volatile uint16_t *src = &VGA_BUFFER[lines * VGA_WIDTH];

    for (size_t y = lines; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            *dest++ = *src++;
        }
    }

    // final lines are cleared
    for (size_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }

    terminal_row = (terminal_row - lines) % VGA_HEIGHT;
}

void vga_putchar(char c) {
    switch (c) {
        case '\n': {
            terminal_column = 0;

            if (++terminal_row == VGA_HEIGHT) {
                vga_scroll(3);
            }

            break;
        }
        case '\r': {
            terminal_row = 0;
            break;
        }
        case '\b': {
            if (terminal_row == 0 && terminal_column == 0) return;

            terminal_column--;
            if (terminal_column < 0) {
                terminal_column = VGA_WIDTH - 1;
                terminal_row--;
            }

            terminal_putentryat('\0', terminal_color, terminal_column, terminal_row);
            break;
        }
        case 0x20 ... 0x7E: {
            terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

            if (++terminal_column == VGA_WIDTH) {
                vga_putchar('\n');
            }
            break;
        }
        default: {
            uint8_t value = c;
            vga_putchar('\\');
            vga_putchar('x');
            vga_putchar("0123456789abcdef"[value >> 4]);
            vga_putchar("0123456789abcdef"[value & 0xf]);
            break;
        }
    }
    vga_cursor_update(terminal_column, terminal_row);
}

void vga_write_string(const char *data) {
    while (*data) {
        vga_putchar(*data++);
    }
}

// Found names here: http://www.osdever.net/FreeVGA/vga/crtcreg.htm
#define CURSOR_ADDRESS_REGISTER 0x3D4
#define CURSOR_INDEX_REGISTER   0x3D5

// Cursor address
#define CURSOR_START 0x0A
#define CURSOR_END   0x0B

#define CURSOR_LOCATION_HIGH 0x0E
#define CURSOR_LOCATION_LOW  0x0F

void vga_cursor_enable(size_t start, size_t end) {
    // Writes to CURSOR_ADDRESS_REGISTER first
    // to specify address

    // start determines start of cursor height
    // end determines end of cursor height

    outb(CURSOR_ADDRESS_REGISTER, CURSOR_START);
    // retain bits 6 and 7 because they are reserved.
    outb(CURSOR_INDEX_REGISTER, inb(CURSOR_INDEX_REGISTER) & 0xC0 | (uint8_t) (start & 0x3F));

    outb(CURSOR_ADDRESS_REGISTER, CURSOR_END);
    // retain bits 5, 6, 7 because they are reserved.
    outb(CURSOR_INDEX_REGISTER, inb(CURSOR_INDEX_REGISTER) & 0xE0 | (uint8_t) (end & 0x1f));
}

void vga_cursor_disable(void) {
    outb(CURSOR_ADDRESS_REGISTER, CURSOR_START);
    // set 5th bit to 1 to disable
    outb(CURSOR_INDEX_REGISTER, inb(CURSOR_INDEX_REGISTER) & 0xC0 | 0x20);
}

void vga_cursor_update(size_t x, size_t y) {
    // todo: ensure 8 bits
    uint16_t pos = y * VGA_WIDTH + x;
    outb(CURSOR_ADDRESS_REGISTER, CURSOR_LOCATION_LOW);
    outb(CURSOR_INDEX_REGISTER, (uint8_t) (pos & 0xFF));
    outb(CURSOR_ADDRESS_REGISTER, CURSOR_LOCATION_HIGH);
    outb(CURSOR_INDEX_REGISTER, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t vga_cursor_position(void) {
    uint16_t pos = 0;
    outb(CURSOR_ADDRESS_REGISTER, CURSOR_LOCATION_LOW);
    pos |= inb(CURSOR_INDEX_REGISTER);
    outb(CURSOR_ADDRESS_REGISTER, CURSOR_LOCATION_HIGH);
    pos |= inb(CURSOR_INDEX_REGISTER);
}
