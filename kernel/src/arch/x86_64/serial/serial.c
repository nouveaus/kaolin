#include <stdint.h>

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outw(uint16_t port, uint16_t value) {
    asm volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

void outl(uint16_t port, uint32_t value) {
    asm volatile("outl %d0, %d1" : : "a"(value), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

uint32_t inl(uint16_t port) {
    uint32_t value;
    asm volatile("inl %d1, %d0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}
