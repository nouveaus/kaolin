#pragma once

#include <stdint.h>
#include <stdbool.h>

bool cpu_has_msr(void);
void cpu_get_msr(uint32_t msr, uint32_t *const lo, uint32_t *const hi);
void cpu_set_msr(uint32_t msr, const uint32_t lo, const uint32_t hi);
