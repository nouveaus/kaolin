#pragma once

// Prints the integer given to the terminal
void puti(int num);

// Prints the character given to the terminal
void putc(const char c);

// Prints the string given to the terminal
void puts(const char *s);

// Prints the double given to the terminal
void putd(const double f);

/* Prints the formatted string given to the terminal
 * format specifiers:
 * %s - string
 * %c - character
 * %d - integer
 * %f - double
 */
void krintf(const char *format, ...);
