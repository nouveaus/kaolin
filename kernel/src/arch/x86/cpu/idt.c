#include "idt.h"

struct idt_descriptor interrupt_gate(enum gate_type type, uint8_t privilege_level, uint16_t segment_selector) {
    struct idt_descriptor idt_descriptor = { 0 };

    // present bit must be set for descriptor to be valid
    idt_descriptor.type_attributes |= 1 << 7;

    // privilege level is only 2 bits, we bitwise AND just in case
    idt_descriptor.type_attributes |= (privilege_level & 0x3) << 5;

    idt_descriptor.type_attributes |= type;
    
    idt_descriptor.selector = segment_selector;

    return idt_descriptor;
}
