#pragma once

#include <stdint.h>
#include <stdbool.h>

// Checks whether the cpu supports msr
bool cpu_has_msr(void);

// Get the msr
void cpu_get_msr(const uint32_t msr, uint32_t *const lo, uint32_t *const hi);

// Set the msr
void cpu_set_msr(const uint32_t msr, const uint32_t lo, const uint32_t hi);
