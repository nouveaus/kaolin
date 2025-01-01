#pragma once

#include <stdint.h>

#define DATA_PORT 0x60
#define STATUS_PORT 0x64

// Writes a byte to the serial port
void outb(const uint16_t port,  const uint8_t value);

// Reads a byte from the serial port
uint8_t inb(const uint16_t port);
