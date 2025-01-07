#include "idt.h"

// aligned for performance
__attribute__((aligned(0x10)))
struct interrupt_descriptor interrupt_descriptors[256] = { 0 };

struct idt_descriptor idt_descriptor = {
    .idt_size = sizeof(interrupt_descriptors) - 1,
    .idt_start = (uint32_t)&interrupt_descriptors[0]
};

static uint16_t idt_segment_descriptor(uint16_t index) {
    // index takes up 12 bits
    // RPL = 0
    // Table indicator = 0 (GDT)
    return ((index & 0xFFF) << 3);
}

void setup_interrupt_gate(uint32_t irq, void *base, enum gate_type type, uint8_t privilege_level) {
    struct interrupt_descriptor *interrupt_descriptor = &interrupt_descriptors[irq];

    // present bit must be set for descriptor to be valid
    interrupt_descriptor->type_attributes |= 1 << 7;

    // privilege level is only 2 bits, we bitwise AND just in case
    interrupt_descriptor->type_attributes |= (privilege_level & 0x3) << 5;

    interrupt_descriptor->type_attributes |= type;

    interrupt_descriptor->selector = idt_segment_descriptor(1);

    // address of the routine
    interrupt_descriptor->offset_1 = (uint32_t)base & 0xFFFF;
    interrupt_descriptor->offset_2 = ((uint32_t)base >> 16) & 0xFFFF;
}

void load_idt(void) {
    asm volatile ("lidt %0" :: "m"(idt_descriptor));
    asm volatile ("sti");
}
