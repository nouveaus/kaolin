#include "arch/x86_64/kernel_main.h"

#include <stdint.h>

#include "arch/x86_64/acpi/acpi.h"
#include "arch/x86_64/apic/ioapic.h"
#include "arch/x86_64/apic/lapic.h"
#include "arch/x86_64/cpu/cpuid.h"
#include "arch/x86_64/cpu/idt.h"
#include "arch/x86_64/cpu/msr.h"
#include "arch/x86_64/drivers/keyboard/keyboard.h"
#include "arch/x86_64/drivers/timer/timer.h"
#include "arch/x86_64/klib/klib.h"
#include "arch/x86_64/memory/paging.h"
#include "arch/x86_64/serial/serial.h"
#include "arch/x86_64/usermode/usermode.h"
#include "arch/x86_64/vga/vga.h"
#include "io.h"

static void read_acpi(void);
static void setup_idt(void);

void _Noreturn _die(void) {
    while (1) asm volatile("cli\nhlt" ::);
}

// todo: move this to its own file later
void exception_handler(void) {
    puts("Fatal Error Occurred!");
    _die();
}

void _Noreturn user_main(void) {
    while (1) puts("Hello, world!\n");
}

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected
 * mode.
 */
void _Noreturn kernel_main(struct boot_parameters parameters) {
    vga_initialize();
    uint32_t eax, ebx, ecx, edx;
    // get vendor string
    __cpuid(0, eax, ebx, ecx, edx);
    print_vendor(ebx, ecx, edx);

    paging_init(parameters.pml4);

    heap_init();

    int *test = kmalloc(sizeof(int));
    *test = 2;
    krintf("test: %d\n", *test);
    int *test2 = kmalloc(sizeof(int) * 4096);
    puts("test 2\n");
    for (size_t i = 0; i < 4096; i++) {
        test2[i] = i;
        krintf("%d, ", test2[i]);
    }
    putc('\n');
    free(test);
    free(test2);

    if (!apic_is_supported()) {
        puts("APIC not supported\n");
        _die();
    }

    if (!cpu_has_msr()) {
        puts("MSR not supported\n");
        _die();
    }

    read_acpi();

    if (!map_apic()) {
        puts("Could not map apic to virtual memory!\n");
        _die();
    }
    krintf("pml4 address: %x\n", parameters.pml4);
    enable_apic();
    puts("APIC ENABLED AND MAPPED\n");

    if (!ioapic_map()) {
        puts("Could not map ioapic to virtual memory!\n");
        _die();
    }

    setup_idt();
    puts("Successfully loaded idt\n");

    char message[] = "X Hello world!\n";

    uint64_t test_virtual_address = (KERNEL_MAPPING_ADDRESS | 0xFFFFFFF);
    map_page(test_virtual_address, 0xFFFFFFF, PAGE_PRESENT);
    if (!verify_mapping(test_virtual_address)) {
        puts("Could not page correctly!\n");
        _die();
    }
    free_address(test_virtual_address);
    if (verify_mapping(test_virtual_address)) {
        puts("Could not free virtual address correctly!\n");
        _die();
    }

    puts("Successfully freed virtual address!\n");

    for (size_t i = 0; i < 2; i++) {
        message[0] = '0' + i;
        i = (i + 1) % 10;

        // ! DONT USE FLOATS - THEY DONT WORK FOR SOME REASON

        krintf("%sThe number is: %d, ticks: %d, entry count: %d\n", message, 5,
               get_timer_ticks(), parameters.address_range_count);

        vga_set_color(1 + (i % 6), VGA_COLOR_BLACK);
        //memmap_print_entries(parameters.address_range_count,
        //                     parameters.address_ranges);
//
        asm volatile("int %0" : : "i"(0x80) : "memory");

        ksleep(276447232);
    }
    enter_usermode((void *) user_main);
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
        puts("Could not verify RSDP\n");
        _die();
    }
    puts("Verified RSDP\n");
    if (!rsdt_map()) {
        puts("Could not map rsdt!\n");
        _die();
    }
    if (!rsdt_verify()) {
        puts("Could not verify RSDT\n");
    }
    puts("Verified RSRT\n");

    if (!madt_find()) {
        puts("Could not find MADT\n");
        _die();
    }
    puts("Found MADT\n");

    if (!madt_map()) {
        puts("Could not map madt!\n");
        _die();
    }

    if (!madt_verify()) {
        puts("Could not verify MADT\n");
        _die();
    }
    puts("Verified MADT\n");

    krintf("LAPIC address: %x\n", madt_get_lapic_address());

    size_t count = ioapic_count_entries();
    krintf("Detected %d ioapic on device\n", count);
    struct madt_entry **madt_entries = get_madt_entries();
    for (size_t i = 0; i < count; i++) {
        struct madt_entry *entry = madt_entries[i];
        if (entry->type == 1) {
            struct madt_entry_apicio *apicio_entry =
                    (struct madt_entry_apicio *) entry;
            krintf("APICIO id: %d\n", apicio_entry->ioapic_id);
            krintf("APICIO address: %x\n", apicio_entry->ioapic_address);
        }
    }
}

static void setup_idt(void) {
    int apic_id = apic_get_id();
    krintf("APIC ID: %d\n", apic_id);

    // system timer
    ioapic_set_redirect((uintptr_t *) IOAPIC_VIRTUAL_ADDRESS, 0, 0x20, apic_id);
    // keyboard
    ioapic_set_redirect((uintptr_t *) IOAPIC_VIRTUAL_ADDRESS, 1, 0x21, apic_id);

    for (size_t vector = 0; vector < 32; vector++) {
        setup_interrupt_gate(vector, exception_handler, INTERRUPT_64_GATE, 0,
                             0);
    }

    // we have interrupt after 31 since 0-31 are reserved for errors
    setup_interrupt_gate(0x20, timer_handler, INTERRUPT_64_GATE, 0, 0);
    setup_interrupt_gate(0x21, keyboard_handler, INTERRUPT_64_GATE, 0, 0);
    setup_interrupt_gate(0x80, trap, TRAP_64_GATE, 0, 0);

    load_idt();
}
