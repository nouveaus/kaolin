#pragma once

#include <stdint.h>

// Refer to: https://wiki.osdev.org/Interrupt_Descriptor_Table
struct idt_descriptor {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attributes;
    uint16_t offset_2;
};

enum gate_type {
    TASK_GATE = 0x5,
    INTERUPT_16_GATE = 0x6,
    TRAP_16_GATE = 0x7,
    INTERRUPT_32_GATE = 0xE,
    TRAP_32_GATE = 0xF
};