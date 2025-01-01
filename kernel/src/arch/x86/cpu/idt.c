#include "idt.h"

struct interrupt_descriptor interrupt_descriptors[256] = { 0 };

struct idt_descriptor idt_descriptor = {
    .idt_size = sizeof(interrupt_descriptors),
    .idt_start = &interrupt_descriptors
};

void setup_interrupt_gate(uint32_t irq, uint32_t base, enum gate_type type, uint8_t privilege_level, uint16_t segment_selector) {
    struct interrupt_descriptor *interrupt_descriptor = &interrupt_descriptors[irq];

    // present bit must be set for descriptor to be valid
    interrupt_descriptor->type_attributes |= 1 << 7;

    // privilege level is only 2 bits, we bitwise AND just in case
    interrupt_descriptor->type_attributes |= (privilege_level & 0x3) << 5;

    interrupt_descriptor->type_attributes |= type;
    
    interrupt_descriptor->selector = segment_selector;

    // address of the routine
    interrupt_descriptor->offset_1 = base & 0xFFFF;
    interrupt_descriptor->offset_2 = (base >> 16) & 0xFFFF;
}

void load_idt(void) {
    asm volatile ("lidt %0" :: "m"(idt_descriptor));
}

uint16_t idt_segment_descriptor(uint16_t index) {
    // index takes up 12 bits
    // RPL = 0
    // Table indicator = 0 (GDT)
    return ((index & 0xFFF) << 3);
}