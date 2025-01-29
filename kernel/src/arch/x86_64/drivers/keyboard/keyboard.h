#pragma once

#include <stddef.h>

struct interrupt_frame;

// Initialise the keyboard driver
void keyboard_init(void);

char getc(void);

size_t getstr(char *str, size_t len);
