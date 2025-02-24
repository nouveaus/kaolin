#pragma once

#include <stdint.h>

// Writes a byte to the serial port
void outb(uint16_t port, uint8_t value);

// Writes a word to the serial port
void outw(uint16_t port, uint16_t value);

// Writes a long to the serial port
void outl(uint16_t port, uint32_t value);

// Reads a byte from the serial port
uint8_t inb(uint16_t port);

// Reads a long from the serial port
uint32_t inl(uint16_t port);
