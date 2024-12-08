#include "vga.h"

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
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
            vga_putchar("0123456789abcdef"[value & 0xff]);
            break;
        }
    }
}

void vga_write_string(const char *data) {
    while (*data) {
        vga_putchar(*data++);
    }
}
