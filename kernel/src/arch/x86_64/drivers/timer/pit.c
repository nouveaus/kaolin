#include "timer.h"

#include "../../apic/ioapic.h"
#include "../../apic/lapic.h"
#include "../../serial/serial.h"

#include "../../../../io.h"

// frequency of PIT oscillator (rounded up)
#define FREQUENCY_BASE 1193182

// frequency used by cga video controller - according to osdev
#define VIDEO_FREQUENCY 3579545

static uint32_t timer_fractions = 0;
static uint32_t system_timer_fractions = 0;

static uint32_t timer_ms = 0;
static uint32_t system_timer_ms = 0;

static uint32_t timer_frequency = 0;

// todo: consider multithreaded
static uint32_t timer_countdown = 0;

#define CHANNEL_0_PORT        0x40
#define CHANNEL_1_PORT        0x41
#define CHANNEL_2_PORT        0x42
#define MODE_COMMAND_REGISTER 0x43


static uint32_t round_frequency(uint32_t frequency) {
    // round it
    uint32_t quotient = VIDEO_FREQUENCY / frequency;
    uint32_t remainder = VIDEO_FREQUENCY % frequency;
    if (remainder >= VIDEO_FREQUENCY / 2) {
        quotient++;
    }

    remainder = quotient % 3;
    quotient /= 3;

    if (remainder >= 3 / 2) {
        quotient++;
    }

    return quotient;
}

// ! investigate why it triple faults when cli and sti
// From: https://wiki.osdev.org/Programmable_Interval_Timer
void pit_init(uint32_t frequency) {

    uint32_t reload_value;
    if (frequency >= FREQUENCY_BASE) {
        reload_value = FREQUENCY_BASE;
    } else if (frequency <= 18) {
        reload_value = 0x10000;
    } else {
        reload_value = round_frequency(frequency);
    }

    timer_frequency = round_frequency(frequency);

    // time in ms * 2^32 = reload_value * 3000 * (2^42) / 3579545 / (2^10)

#define MULTIPLIER (3000 * (2ul << 42) / 3579545)
    uint64_t quotient = (reload_value * MULTIPLIER) << 10;

    timer_ms = (quotient >> 32) & 0xFFFFFFFF;
    timer_fractions = quotient & 0xFFFFFFFF;

    // operating mode: rate generator
    outb(MODE_COMMAND_REGISTER, 0b00110100);


    // 16 bits total

    // program the pit
    outb(CHANNEL_0_PORT, reload_value & 0xFF);
    outb(CHANNEL_0_PORT, (reload_value >> 8) & 0xFF);

}

uint32_t timer_get_actual_frequency(void) {
    return timer_frequency;
}

void pleep(uint32_t millis) {
    timer_countdown = millis;
    while (timer_countdown > 0) {
        __builtin_ia32_pause();
    }
}

// ! currently not working
__attribute__((interrupt)) void pit_handler(struct interrupt_frame *frame) {
    system_timer_fractions += timer_fractions;
    puts("TIMER ACTIVATED\n");
    system_timer_ms += timer_ms;
    if (timer_countdown > 0) timer_countdown -= timer_ms;
    send_apic_eoi();
}
