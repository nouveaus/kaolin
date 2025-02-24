#include <stdint.h>

#include "../../serial/serial.h"

#include "../../klib/klib.h"

// frequency of PIT oscillator
#define FREQUENCY_BASE 1193182
#define SPEAKER_PORT 0x61

// todo: cleanup code - make function to communicate to channel 2 for pit
#define CHANNEL_0_PORT        0x40
#define CHANNEL_1_PORT        0x41
#define CHANNEL_2_PORT        0x42
#define MODE_COMMAND_REGISTER 0x43

static void speaker_play_sound(uint32_t frequency) {
    uint32_t div = FREQUENCY_BASE / frequency;


    outb(MODE_COMMAND_REGISTER, 0b10110110);

    outb(CHANNEL_2_PORT, div & 0xFF);
    outb(CHANNEL_2_PORT, (div >> 8) & 0xFF);

    // bit 0 = use PIT
    // bit 1 = out position
    outb(SPEAKER_PORT, inb(SPEAKER_PORT) | 0x3);
}

static void speaker_no_sound(void) {
    outb(SPEAKER_PORT, inb(SPEAKER_PORT) & ~0x3);
}

void beep(uint32_t frequency) {
    speaker_play_sound(frequency);
    ksleep(1000000);
    speaker_no_sound();
}
