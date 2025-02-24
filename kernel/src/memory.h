#pragma once

#include <stddef.h>

// Compare each byte of the two addresses given the count
int memcmp(const void *a, const void *b, size_t count);

// Sets each byte in destination given the value and size
void memset(void *dst, int val, size_t size);

// Copies the memory from source to destination given the length
void *memcpy(void *dst, const void *src, size_t len);

// Moves the memory from source to destination given the length
// (Safe for overlapping)
void *memmove(void *dst, const void *src, size_t len);
