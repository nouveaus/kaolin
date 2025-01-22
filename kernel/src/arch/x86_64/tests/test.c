#include "../serial/serial.h"

#define QEMU_EXIT_PORT 0xf4

_Noreturn void test_fail(void) {
    outw(QEMU_EXIT_PORT, 0x11);

    while (1) {
        asm volatile("cli\n"
                     "hlt" ::);
    }
}
