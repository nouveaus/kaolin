#include "keyboard.h"

#include "../../apic/lapic.h"
#include "../../serial/serial.h"
#include "../../../../io.h"

__attribute__((interrupt)) void keyboard_handler(struct interrupt_frame* frame) {
    // Consume a key, not doing this will only let
    // the interrupt trigger once
    inb(0x60);
    puts("Keyboard pressed");
    send_apic_eoi();
}
