#include "klib.h"

#include "../apic/lapic.h"

void ksleep(int ticks) {
    init_apic_timer(ticks, 0x20);
    while (get_apic_timer_current());
}

int memcmp(const void *a, const void *b, size_t count) {
    const unsigned char *s1 = (const unsigned char *) a;
    const unsigned char *s2 = (const unsigned char *) b;

    for (size_t i = 0; i < count; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
    }

    return 0;
}

// how have we gone so far without this??
void memset(void *dst, char val, size_t size) {
    for (size_t i = 0; i < size; i++) {
        ((char *) dst)[i] = val;
    }
}

void *memcpy(void *dst, const void *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ((char *) dst)[i] = ((const char *) src)[i];
    }

    return dst;
}

void *memmove(void *dst, const void *src, size_t len) {
    if (dst > src) {
        for (size_t i = len; i > 0; i--) {
            ((char *) dst)[i - 1] = ((const char *) src)[i - 1];
        }
    } else {
        memcpy(dst, src, len);
    }

    return dst;
}

_Noreturn void _die(void) {
    while (1) {
        asm volatile("cli\nhlt" ::);
    }
}
