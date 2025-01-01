#pragma once

#include <stdint.h>

// Refer to: https://wiki.osdev.org/Interrupt_Descriptor_Table
struct interrupt_descriptor {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attributes;
    uint16_t offset_2;
} __attribute__((packed));

struct idt_descriptor {
    uint16_t idt_size;
    uint32_t idt_start;
} __attribute__((packed));

enum gate_type {
    TASK_GATE = 0x5,
    INTERUPT_16_GATE = 0x6,
    TRAP_16_GATE = 0x7,
    INTERRUPT_32_GATE = 0xE,
    TRAP_32_GATE = 0xF
};

void setup_interrupt_gate(uint32_t irq, enum gate_type type, uint8_t privilege_level, uint16_t segment_selector);

void load_idt(void);

uint16_t idt_segment_descriptor(uint16_t index);
