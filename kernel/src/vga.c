#include "vga.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color;
volatile uint16_t *terminal_buffer = (volatile uint16_t *)ADDR_VIDEO_MEMORY;

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    // using enums no masking needed
    return (bg << 4) | fg;
}

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) (color << 8) | c;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

void terminal_initialize(void) {
    terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    terminal_buffer[y * VGA_WIDTH + x] = vga_entry(c, color);
}

void terminal_scroll(void) {
    // no clue if this works
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            char c = terminal_buffer[y * VGA_WIDTH + x];
            terminal_buffer[(y - 1) * VGA_WIDTH + x] = vga_entry(c, terminal_color);
        }
    }
}

void terminal_putchar(char c) {
    if (c == '\n') goto newline;
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH) {
        newline:
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) terminal_scroll();
    }
}

void terminal_write(const char *data, size_t size) {
    for (size_t i = 0; i < size; i++) terminal_putchar(data[i]);
}

inline void terminal_writestring(const char *data) {
    terminal_write(data, strlen(data));
}