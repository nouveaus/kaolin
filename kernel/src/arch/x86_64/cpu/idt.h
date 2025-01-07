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
    uint64_t idt_start;
} __attribute__((packed));

enum gate_type {
    TASK_GATE = 0x5,
    INTERUPT_16_GATE = 0x6,
    TRAP_16_GATE = 0x7,
    INTERRUPT_32_GATE = 0xE,
    TRAP_32_GATE = 0xF
};

struct interrupt_frame {
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t sp;
    uint32_t ss;
};

void setup_interrupt_gate(uint32_t irq, void *base, enum gate_type type, uint8_t privilege_level);

void load_idt(void);
