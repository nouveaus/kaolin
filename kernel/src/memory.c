#include "memory.h"

int memcmp(const void *a, const void *b, size_t count) {
    const unsigned char *s1 = a;
    const unsigned char *s2 = b;

    for (size_t i = 0; i < count; i++) {
        if (s1[i] != s2[i]) return s1[i] - s2[i];
    }

    return 0;
}

void memset(void *dst, int val, size_t size) {
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
