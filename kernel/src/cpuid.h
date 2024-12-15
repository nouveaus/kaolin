#pragma once

#include <stdbool.h>
#include <stdint.h>

bool cpuid_is_supported(void);
#define CHAR_BIT 8

void print_reg(uint32_t ebx);
void print_vendor(uint32_t ebx, uint32_t ecx, uint32_t edx);
void call_cpuid(int eax);
