#include "timer.h"

#include "../../apic/ioapic.h"

__attribute__((interrupt)) void pit_handler(struct interrupt_frame *frame) {
    
    send_apic_eoi();
}
