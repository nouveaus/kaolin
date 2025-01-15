#pragma once

#include <stdint.h>
#include <stddef.h>

void heap_init(uint64_t *pml4);
void *kmalloc(size_t size);
void free(void *address);
