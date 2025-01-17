#pragma once

#include <stddef.h>
#include <stdint.h>

enum vga_color {
    VGA_COLOR_BLACK,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_YELLOW,
    VGA_COLOR_WHITE
};

// Initializes the global VGA handler. Must be called once before any other VGA calls
void vga_initialize(void);

// Changes the color of any text printed next
void vga_set_color(enum vga_color fg, enum vga_color bg);

// Scrolls down the given number of lines
void vga_scroll(int lines);

// Prints a single character
void vga_putchar(char c);

// Prints a null-terminated string
void vga_write_string(const char *data);
