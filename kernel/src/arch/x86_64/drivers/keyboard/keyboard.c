#include "keyboard.h"

#include "../../../../io.h"
#include "../../apic/ioapic.h"
#include "../../apic/lapic.h"
#include "../../cpu/idt.h"
#include "../../klib/klib.h"
#include "../../serial/serial.h"

#include <stdbool.h>

#define DATA_PORT        0x60
#define STATUS_REGISTER  0x64
#define COMMAND_REGISTER STATUS_REGISTER

#define SET_LEDS 0xED

// clang-format off
enum scan_set_one {
    // todo: abbreviate names
    S1_ESC = 1, S1_1, S1_2, S1_3, S1_4,
    S1_5, S1_6, S1_7, S1_8, S1_9,
    S1_0, S1_MINUS, S1_EQUAL, S1_BACKSPACE, S1_TAB,
    S1_Q, S1_W, S1_E, S1_R, S1_T,
    S1_Y, S1_U, S1_I, S1_O, S1_P, S1_LEFT_BRACKET,
    S1_RIGHT_BRACKET, S1_ENTER, S1_LEFT_CTRL, S1_A, S1_S,
    S1_D, S1_F, S1_G, S1_H, S1_J,
    S1_K, S1_L, S1_SEMI, S1_SINGLE_QUOTE, S1_BACK_TICK,
    S1_LEFT_SHIFT, S1_LEFT_SLASH, S1_Z, S1_X, S1_C,
    S1_V, S1_B, S1_N, S1_M, S1_COMMA,
    S1_DOT, S1_RIGHT_SLASH, S1_RIGHT_SHIFT, S1_KEYPAD_STAR,
    S1_LEFT_ALT, S1_SPACE, S1_CAPSLOCK, S1_F1, S1_F2,
    S1_F3, S1_F4, S1_F5, S1_F6, S1_F7,
    S1_F8, S1_F9, S1_F10, S1_NUMLOCK, S1_SCROLLLOCK,
    S1_KEYPAD_7, S1_KEYPAD_8, S1_KEYPAD_9, S1_KEYPAD_MINUS,
    S1_KEYPAD_4, S1_KEYPAD_5, S1_KEYPAD_6, S1_KEYPAD_PLUS,
    S1_KEYPAD_1, S1_KEYPAD_2, S1_KEYPAD_3, S1_KEYPAD_0,
    S1_KEYPAD_DOT, S1_F11, S1_F12,
    // RELEASED
    // todo: make a map for each key held down
    S1_ESC_RELEASED = 0x81,
    S1_1_RELEASED, S1_2_RELEASED, S1_3_RELEASED, S1_4_RELEASED,
    S1_5_RELEASED, S1_6_RELEASED, S1_7_RELEASED, S1_8_RELEASED,
    S1_9_RELEASED, S1_0_RELEASED, S1_MINUS_RELEASED, S1_EQUALS_RELEASED,
    S1_BACKSPACE_RELEASED, S1_TAB_RELEASED, S1_Q_RELEASED, S1_W_RELEASED,
    S1_E_RELEASED, S1_R_RELEASED, S1_T_RELEASED, S1_Y_RELEASED, S1_U_RELEASED,
    S1_I_RELEASED, S1_O_RELEASED, S1_P_RELEASED, S1_LEFT_BRACKET_RELEASED,
    S1_RIGHT_BRACKET_RELEASED, S1_ENTER_RELEASED, S1_LEFT_CTRL_RELEASED, S1_A_RELEASED,
    S1_S_RELEASED, S1_D_RELEASED, S1_F_RELEASED, S1_G_RELEASED, S1_H_RELEASED,
    S1_J_RELEASED, S1_K_RELEASED, S1_L_RELEASED, S1_SEMI_RELEASED, S1_SINGLE_QUOTE_RELEASED,
    S1_BACK_TICK_RELEASED, S1_LEFT_SHIFT_RELEASED, S1_LEFT_SLASH_RELEASED, S1_Z_RELEASED,
    S1_X_RELEASED, S1_C_RELEASED, S1_V_RELEASED, S1_B_RELEASED, S1_N_RELEASED,
    S1_M_RELEASED, S1_COMMA_RELEASED, S1_DOT_RELEASED, S1_RIGHT_SLASH_RELEASED,
    S1_RIGHT_SHIFT_RELEASED, S1_KEYPAD_STAR_RELEASED, S1_LEFT_ALT_RELEASED, S1_SPACE_RELEASED,
    S1_CAPSLOCK_RELEASED, S1_F1_RELEASED, S1_F2_RELEASED, S1_F3_RELEASED, S1_F4_RELEASED,
    S1_F5_RELEASED, S1_F6_RELEASED, S1_F7_RELEASED, S1_F8_RELEASED, S1_F9_RELEASED,
    S1_F10_RELEASED, S1_NUMLOCK_RELEASED, S1_SCROLLLOCK_RELEASED, S1_KEYPAD_7_RELEASED,
    S1_KEYPAD_8_RELEASED, S1_KEYPAD_9_RELEASED, S1_KEYPAD_MINUS_RELEASED,
    S1_KEYPAD_4_RELEASED, S1_KEYPAD_5_RELEASED, S1_KEYPAD_6_RELEASED, S1_KEYPAD_PLUS_RELEASED,
    S1_KEYPAD_1_RELEASED, S1_KEYPAD_2_RELEASED, S1_KEYPAD_3_RELEASED, S1_KEYPAD_0_RELEASED,
    S1_KEYPAD_DOT_RELEASED, S1_F11_RELEASED, S1_F12_RELEASED
};
// clang-format on

#define MAX_BUFFER_SIZE 512
static char buffer[MAX_BUFFER_SIZE];
static volatile size_t buffer_pos = 0;

static __attribute__((interrupt)) void keyboard_handler(struct interrupt_frame *frame);

static uint8_t keyboard_code_set_1(const uint8_t data);
static uint8_t keyboard_shift_conversion(uint8_t data);

static bool caps_lock = false;
static bool shift_pressed = false;

// TODO: use acpi to check if 8042 ps/2 controller is supported
// todo: poll for keyboard status
// todo: check keyboard type (1, 2, 3)

void keyboard_init(void) {
    // Disable any devices connected to ps/2 ports
    //outb(0x64, 0xAD);
    //outb(0x64, 0xA7);
    // todo: do the steps specified by osdev later

    int apic_id = apic_get_id();
    ioapic_set_redirect((uintptr_t *) IOAPIC_VIRTUAL_ADDRESS, 1, 0x21, apic_id);
    setup_interrupt_gate(0x21, keyboard_handler, INTERRUPT_64_GATE, 0, 0);
}

// Keycodes from: https://wiki.osdev.org/PS/2_Keyboard
// CODE SET  1
// ! for some reason 2,3,4 in qemu r different keys
static uint8_t keyboard_code_set_1(const uint8_t data) {
    if (data >= S1_1 && data <= S1_BACKSPACE) {
        return "1234567890-=\b"[data - S1_1];
    } else if (data >= S1_Q && data <= S1_ENTER) {
        return "QWERTYUIOP[]\n"[data - S1_Q];
    } else if (data >= S1_A && data <= S1_BACK_TICK) {
        return "ASDFGHJKL;'`"[data - S1_A];
    } else if (data >= S1_LEFT_SLASH && data <= S1_RIGHT_SLASH) {
        return "\\ZXCVBNM,./"[data - S1_LEFT_SLASH];
    } else if (data >= S1_KEYPAD_7 && data <= S1_KEYPAD_DOT) {
        return "789-456+1230."[data - S1_KEYPAD_7];
    } else if (data == S1_SPACE) {
        return ' ';
    } else if (data == S1_KEYPAD_STAR) {
        return '*';
    } else if (data == S1_CAPSLOCK) {
        caps_lock = !caps_lock;
        // enable caps lock LED
        if (caps_lock) {
            outb(DATA_PORT, SET_LEDS);
            ksleep(5);
            outb(DATA_PORT, 1 << 2);
        } else {
            outb(DATA_PORT, SET_LEDS);
            ksleep(5);
            outb(DATA_PORT, 0x0);
        }
    } else if (data == S1_LEFT_SHIFT || data == S1_RIGHT_SHIFT) {
        shift_pressed = true;
    } else if (data == S1_LEFT_SHIFT_RELEASED || data == S1_RIGHT_SHIFT_RELEASED) {
        shift_pressed = false;
    }

    return data;
}

// TODO: have switch for different code sets
static char keyboard_convert_code(char data) {
    return keyboard_code_set_1(data);
}

// ! figure out how to make it pause/wait for getchar();

// todo: WRITE A FRONTEND FUNCTION FOR KLIB
char getc(void) {
    while (!buffer_pos) {
        __builtin_ia32_pause();
    }
    char data = buffer[--buffer_pos];
    putc(data);
    return data;
}

size_t getstr(char *str, size_t len) {
    size_t i = 0;
    while (i < len) {
        char data = getc();
        if (data == '\n') return i;
        if (data == '\b') {
            i--;
            continue;
        }
        str[i] = data;
        i++;
    }
    return i;
}

static uint8_t keyboard_shift_conversion(uint8_t data) {
    if (!shift_pressed) return data;

    // Inverse because of caps lock
    if (data >= 'A' && data <= 'Z') return data + 32;
    if (data >= 'a' && data <= 'z') return data - 32;

    if (data >= '0' && data <= '9') return ")!@#$%^&*("[data - '0'];

    switch (data) {
        case '[':
            return '{';
        case ']':
            return '}';
        case '-':
            return '_';
        case '=':
            return '+';
        case '\\':
            return '|';
        case ';':
            return ':';
        case ',':
            return '<';
        case '.':
            return '>';
        default:
            return data;
    }
}

static void keyboard_handler(struct interrupt_frame *frame) {
    // Consume a key, not doing this will only let
    // the interrupt trigger once
    uint8_t data = keyboard_code_set_1(inb(DATA_PORT));
    if (data >= 'A' && data <= 'Z' && !caps_lock) data += 32;
    data = keyboard_shift_conversion(data);
    if ((data >= ' ' && data <= '~') || data == '\b' || data == '\t' || data == '\n') buffer[buffer_pos++] = data;
    // ! minor bug: could loop over and getc wont be able to get the prev 512 chars
    buffer_pos %= MAX_BUFFER_SIZE;
    //if (data >= ' ' && data <= '~')
    //    krintf("Keyboard pressed %c %d\n", data, buffer_pos);
    send_apic_eoi();
}
