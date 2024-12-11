#pragma once

#define CHAR_BIT 8

#include <stdint.h>


void print_reg(uint32_t ebx);
void print_vendor(uint32_t ebx, uint32_t ecx, uint32_t edx);
void call_cpuid(int eax);
