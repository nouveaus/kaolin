#pragma once

#include <stdint.h>

// Returns the timer
uint32_t apic_timer_get_ticks(void);

struct interrupt_frame;

// Timer handler
void apic_timer_handler(struct interrupt_frame *frame);

// Initialises the apic timer
void apic_timer_init(uint32_t initial_count, uint8_t vector);

// Get the current count of apic timer
uint32_t apic_timer_get_current(void);

void pit_handler(struct interrupt_frame *frame);

void pit_init(uint32_t frequency);

uint32_t timer_get_actual_frequency(void);

// Sleep but from PIT timer
void pleep(uint32_t millis);
