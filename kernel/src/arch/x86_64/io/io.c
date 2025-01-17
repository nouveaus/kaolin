#include "../../../io.h"
#include "../serial/serial.h"
#include "../vga/vga.h"

#define BASE       0x3F8
#define DATA       BASE
#define INT_EN     (BASE + 1)
#define FIFO_CTRL  (BASE + 2)
#define LINE_CTRL  (BASE + 3)
#define MODEM_CTRL (BASE + 4)
#define LINE_STS   (BASE + 5)

#define STS_OUTPUT_EMPTY (1 << 5)

int serial = 0;

void enable_serial_output(void) {
    if (!serial) {
        // Disable interrupts
        outb(INT_EN, 0x80);

        // Enable DLAB
        outb(LINE_CTRL, 0x80);

        // Set maximum speed to 38400 bps by configuring DLL and DLM
        outb(DATA, 0x03);
        outb(INT_EN, 0x00);

        // Disable DLAB and set data word length to 8 bits
        outb(LINE_CTRL, 0x03);

        // Enable FIFO, clear TX/RX queues and
        // set interrupt watermark at 14 bytes
        outb(FIFO_CTRL, 0x67);

        // Mark data terminal ready, signal request to send
        // and enable auxilliary output #2 (used as interrupt line for CPU)
        outb(MODEM_CTRL, 0x0B);

        // Enable interrupts
        outb(INT_EN, 0x01);

        serial = 1;
    }
}

void putc(char c) {
    vga_putchar(c);

    if (serial) {
        while (!(inb(LINE_STS) & STS_OUTPUT_EMPTY)) {
            __builtin_ia32_pause();
        }

        outb(DATA, c);
    }
}

void puts(const char *s) {
    while (*s) {
        putc(*s++);
    }
}
