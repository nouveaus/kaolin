typedef unsigned short int uint16_t;

void _Noreturn kernel_main(void) {
    volatile uint16_t *vga = (volatile uint16_t *)0xB8000;
    unsigned int i = 0;

    while (1) {
        vga[0] = 'A' + i | 0x0F00;
        vga[120] = 'J' + i | 0x0F00;
        vga[259] = 'P' + i | 0x0F00;

        i = 1 - i;
    }
}
