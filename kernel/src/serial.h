#pragma once

#include <stdint.h>

#define DATA_PORT 0x60
#define STATUS_PORT 0x64

void outb(const uint16_t port,  const uint8_t value);
uint8_t inb(const uint16_t port);
