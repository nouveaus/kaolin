#include "keyboard.h"

#include "../../apic/lapic.h"
#include "../../serial/serial.h"
#include "../../../../io.h"


void keyboard_handler(void) {
    //asm volatile ("pusha\n");
    // Consume a key, not doing this will only let
    // the interrupt trigger once
    inb(0x60);
    puts("Keyboard pressed");
    send_apic_eoi();
    asm volatile (/*"popa\n*/"leave\niretq");
}
