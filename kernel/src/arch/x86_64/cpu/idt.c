#include "idt.h"

// aligned for performance
__attribute__((aligned(0x10)))
struct interrupt_descriptor interrupt_descriptors[256] = { 0 };

struct idt_descriptor idt_descriptor = {
    .idt_size = sizeof(interrupt_descriptors) - 1,
    .idt_start = 0,
};

static uint16_t idt_segment_descriptor(uint16_t index) {
    // index takes up 12 bits
    // RPL = 0
    // Table indicator = 0 (GDT)
    return ((index & 0xFFF) << 3);
}

void setup_interrupt_gate(uint32_t irq, void *base, enum gate_type type, uint8_t privilege_level, uint8_t ist) {
    struct interrupt_descriptor *interrupt_descriptor = &interrupt_descriptors[irq];

    // present bit must be set for descriptor to be valid
    interrupt_descriptor->type_attributes |= 1 << 15;

    // privilege level is only 2 bits, we bitwise AND just in case
    interrupt_descriptor->type_attributes |= (privilege_level & 0x3) << 13;

    interrupt_descriptor->type_attributes |= type << 8;

    interrupt_descriptor->type_attributes |= ist;

    interrupt_descriptor->selector = idt_segment_descriptor(1);

    // address of the routine
    interrupt_descriptor->offset_low = (uint64_t)base & 0xFFFF;
    interrupt_descriptor->offset_mid = ((uint64_t)base >> 16) & 0xFFFF;
    interrupt_descriptor->offset_high = ((uint64_t)base >> 32) & 0xFFFFFFFF;
}

void load_idt(void) {
    // moved to here because compiler was giving a warning
    // about static initialisation or something
    idt_descriptor.idt_start = (uint64_t)&interrupt_descriptors[0];
    asm volatile ("lidt %0" :: "m"(idt_descriptor));
    asm volatile ("sti");
}
