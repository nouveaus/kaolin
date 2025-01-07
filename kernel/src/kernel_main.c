#include "arch/x86/kernel_main.h"
#include "arch/x86/vga/vga.h"
#include "arch/x86/cpu/cpuid.h"
#include "arch/x86/cpu/msr.h"
#include "arch/x86/cpu/idt.h"
#include "arch/x86/cpu/long_mode.h"
#include "arch/x86/cpu/gdt.h"
#include "arch/x86/apic/lapic.h"
#include "arch/x86/apic/ioapic.h"
#include "arch/x86/acpi/acpi.h"
#include "arch/x86/serial/serial.h"
#include "arch/x86/klib/klib.h"
#include "arch/x86/drivers/keyboard/keyboard.h"
#include "arch/x86/drivers/timer/timer.h"
#include "io.h"

static void read_acpi(void);
static void setup_idt(void);

void _Noreturn _die(void) { while(1) asm volatile("cli\nhlt" ::); }

void exception_handler(void) {
    //asm volatile ("pusha\n");
    puts("Fatal Error Occurred!");
    _die();
}

void _Noreturn kernel_main_64() {
    puts("entered long mode!!!");
    _die();
}

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected mode.
 */
void _Noreturn kernel_main(struct boot_parameters parameters) {
    vga_initialize();
    vga_putchar('a');
    vga_putchar('b');

    uint32_t eax, ebx, ecx, edx;
    // get vendor string
    __cpuid(0, eax, ebx, ecx, edx);
    print_vendor(ebx, ecx, edx);

    if (!apic_is_supported()) {
        puts("APIC not supported\n");
        _die();
    }

    if (!cpu_has_msr()) {
        puts("MSR not supported\n");
        _die();
    }

    read_acpi();

    puts("test");
    _die();

    if (!long_mode_is_supported()) {
        puts("Long mode is not supported\n");
        _die();
    }
    
    setup_paging();
    enable_long_mode();
    enter_long_mode(kernel_main_64);
    
    // enable_apic();

    // setup_idt();
    
    /*
    char message[] = "X Hello world!\n";
    unsigned int i = 0;

    while (1) {
        message[0] = '0' + i;
        i = (i + 1) % 10;

        //vga_write_string(message);
        krintf("%sThe number is: %d, float is: %f, ticks: %d, entry count: %d\n", message, 5, 3.9999, get_timer_ticks(), parameters.address_range_count);
        vga_set_color(1 + (i % 6), VGA_COLOR_BLACK);
        memmap_print_entries(parameters.address_range_count, parameters.address_ranges);
        asm volatile ("int %0" : : "i"(0x80) : "memory");

        ksleep(276447232);
    }
    */
}

static void read_acpi(void) {
    if (!rsdp_find()) {
        puts("Could not find RSDP\n");
        _die();
    }

    puts("Found RSDP\nRSDP signature:");
    rsdp_print_signature();
    krintf("RSDP revision: %d\n", rsdp_get_revision());
    if (!rsdp_verify()) {
        krintf("Could not verify RSDP\n");
        _die();
    }
    puts("Verified RSDP\n");

    if (!rsdt_verify()) {
        puts("Could not verify RSDT\n");
    }
    puts("Verified RSRT\n");

    if (!madt_find()) {
        puts("Could not find MADT\n");
        _die();
    }

    puts("Found MADT\n");

    if (!madt_verify()) {
        puts("Could not verify MADT\n");
        _die();
    }
    puts("Verified MADT\n");

    krintf("LAPIC address: %x\n", madt_get_lapic_address());

    size_t count = ioapic_get_entry_count();
    krintf("Detected %d ioapic on device\n", count);

    krintf("Address is %x\n", get_first_ioapic_address());
}

static void setup_idt(void) {
    int apic_id = apic_get_id();
    krintf("APIC ID: %d\n", apic_id);

    // system timer
    ioapic_set_redirect((uintptr_t*)IOAPICBASE, 0, 0x20, apic_id);
    // keyboard
    ioapic_set_redirect((uintptr_t*)IOAPICBASE, 1, 0x21, apic_id);

    for (size_t vector = 0; vector < 32; vector++) {
        setup_interrupt_gate(vector, exception_handler, INTERRUPT_32_GATE, 0);
    }

    // we have interrupt after 31 since 0-31 are reserved for errors
    setup_interrupt_gate(0x20, timer_handler, INTERRUPT_32_GATE, 0);
    setup_interrupt_gate(0x21, keyboard_handler, INTERRUPT_32_GATE, 0);
    setup_interrupt_gate(0x80, trap, TRAP_32_GATE, 0);

    load_idt();
}
