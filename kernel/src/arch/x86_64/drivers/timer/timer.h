#pragma once

#include <stdint.h>

// Returns the timer
uint32_t get_timer_ticks(void);

struct interrupt_frame;

// Timer handler
void timer_handler(struct interrupt_frame* frame);
