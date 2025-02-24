#include <stdint.h>

#include "../../serial/serial.h"

// frequency of PIT oscillator
#define FREQUENCY_BASE 1193180

#define SPEAKER_PORT 0x61


static void speaker_play_sound(uint32_t frequency) {
    uint32_t div = FREQUENCY_BASE / frequency;

}
