#include "mouse.h"


#include "../../../../io.h"
#include "../../apic/ioapic.h"
#include "../../apic/lapic.h"
#include "../../cpu/idt.h"
#include "../../klib/klib.h"
#include "../../serial/serial.h"

#define DATA_PORT        0x60
#define STATUS_REGISTER  0x64
#define COMMAND_REGISTER STATUS_REGISTER


#define MOUSE_COMMAND          0xD4
#define MOUSE_BIT              0x20
#define GET_COMPAQ_STATUS_BYTE 0x20

static bool right_button = false;
static bool left_button = false;
static bool middle_button = false;
static bool x_sign = false;
static bool y_sign = false;
static uint8_t x_movement = 0;
static uint8_t y_movement = 0;

static __attribute__((interrupt)) void mouse_handler(struct interrupt_frame *frame);
static void wait_mouse(bool is_output);

void mouse_init(void) {
    // enable mouse
    wait_mouse(true);
    outb(COMMAND_REGISTER, 0xA8);

    // enable aux port to generate irq 12 (according to osdev)
    wait_mouse(true);
    outb(COMMAND_REGISTER, GET_COMPAQ_STATUS_BYTE);

    wait_mouse(false);
    uint8_t status = inb(STATUS_REGISTER);
    status |= 0x10; // Enable IRQ12
    status ^= ~0x20;// Disable mouse clock

    // todo: bundle wait_mouse and outb/inb into one function

    // update status with compaq byte
    wait_mouse(true);
    outb(COMMAND_REGISTER, 0x60);
    wait_mouse(true);
    outb(DATA_PORT, status);

    // set defaults for mouse
    wait_mouse(true);
    outb(COMMAND_REGISTER, MOUSE_COMMAND);
    wait_mouse(true);
    outb(DATA_PORT, 0xF6);

    // enable packet streaming
    wait_mouse(true);
    outb(COMMAND_REGISTER, MOUSE_COMMAND);
    wait_mouse(true);
    outb(DATA_PORT, 0xF4);

    int apic_id = apic_get_id();
    ioapic_set_redirect((uintptr_t *) IOAPIC_VIRTUAL_ADDRESS, 12, 0x22, apic_id);
    setup_interrupt_gate(0x22, mouse_handler, INTERRUPT_64_GATE, 0, 0);
}


// See: https://wiki.osdev.org/Mouse_Input#Waiting_to_Send_Bytes_to_Port_0x60_and_0x64
static void wait_mouse(bool is_output) {
    if (is_output) {
        while (inb(STATUS_REGISTER) & 0x2) {
            __builtin_ia32_pause();
        }
    } else {
        while (!(inb(DATA_PORT) & 0x1)) {
            __builtin_ia32_pause();
        }
    }
}

static void mouse_handler(struct interrupt_frame *frame) {
    uint8_t status = inb(STATUS_REGISTER);
    puts("Mouse interrupt\n");
    size_t nth_byte = 0;

    while (status & MOUSE_BIT) {
        uint8_t data = inb(DATA_PORT);
        switch (nth_byte) {
            case 0:
                right_button = data & 0x1;
                left_button = data & 0x2;
                middle_button = data & 0x4;
                x_sign = data & 0x10;
                y_sign = data & 0x20;
                break;
            case 1:
                x_movement = data;
                break;
            case 2:
                y_movement = data;
                break;
        }
        nth_byte++;
        nth_byte %= 3;
        krintf(
                "Mouse info\n"
                "right button: %d\n"
                "left button: %d\n"
                "middle button: %d\n"
                "x sign: %d\n"
                "y sign: %d\n"
                "x movement: %d\n"
                "y movement: %d\n",
                right_button, left_button, middle_button, x_sign, y_sign, x_movement, y_movement);
        status = inb(STATUS_REGISTER);
    }
    puts("Finished mouse interrupt\n");
    send_apic_eoi();
}
