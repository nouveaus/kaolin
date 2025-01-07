#pragma once

#include <stdint.h>

// Returns the timer
uint32_t get_timer_ticks(void);

// Timer handler
void timer_handler(void);
