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

#ifdef KAOLIN_TESTS
#    include "test.h"

extern struct test_info test_info_start;
extern struct test_info test_info_end;
#endif

static void read_acpi(void);

static void setup_idt(void);

void user_puts(const char *s) {
    register uint64_t syscall asm("rax") = 0;
    register const char *input asm("rdi") = s;

    asm volatile(
            "int $0x80"
            :
            : "r"(syscall), "r"(input)
            :);
}

_Noreturn void user_main(void) {
    const char *msg = "Entered usermode!\n";

    while (1) {
        user_puts(msg);
    }
}

/*
 * The entry point after the bootloader finishes setting up x86 32-bit protected
 * mode.
 */
void kernel_main(struct boot_parameters parameters) {
    paging_init(parameters.address_ranges);

    enable_serial_output();
    vga_initialize();
    uint32_t eax, ebx, ecx, edx;
    // get vendor string
    __cpuid(0, eax, ebx, ecx, edx);
    print_vendor(ebx, ecx, edx);

    heap_init();

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

    enable_apic();
    puts("APIC ENABLED AND MAPPED\n");

    setup_idt();
    puts("Successfully loaded idt\n");

#ifdef KAOLIN_TESTS
    int index = 0;

    for (struct test_info *test = &test_info_start; test != &test_info_end; test++) {
        krintf("\n\nRunning test %d (%s)\n", ++index, test->name);
        (test->test)();

        krintf("\n\nPassed test %d (%s)\n", index, test->name);
    }

    krintf("Passed all tests\n");
    outw(0xf4, 0x10);
    _die();
#else
    char message[] = "X Hello world!\n";

    for (size_t i = 0; i < 1; i++) {
        message[0] = '0' + i;
        i = (i + 1) % 10;

        // ! DONT USE FLOATS - THEY DONT WORK FOR SOME REASON

        krintf("%sThe number is: %d, ticks: %d, entry count: %d\n", message, 5,
               get_timer_ticks(), parameters.address_ranges);

        vga_set_color(1 + (i % 6), VGA_COLOR_BLACK);
        //memmap_print_entries(parameters.address_range_count,
        //                     parameters.address_ranges);
        //

        ksleep(276447232);
    }
    enter_usermode(user_main);
#endif
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
    void *ioapic = ioapic_map();
    int apic_id = apic_get_id();
    krintf("APIC ID: %d\n", apic_id);

    // system timer
    ioapic_set_redirect(ioapic, 0, 0x20, apic_id);
    // keyboard
    ioapic_set_redirect(ioapic, 1, 0x21, apic_id);

    for (size_t vector = 0; vector < 32; vector++) {
        setup_interrupt_gate(vector, exception_handler, INTERRUPT_64_GATE, 0,
                             0);
    }

    // we have interrupt after 31 since 0-31 are reserved for errors
    setup_interrupt_gate(0x20, timer_handler, INTERRUPT_64_GATE, 0, 0);
    setup_interrupt_gate(0x21, keyboard_handler, INTERRUPT_64_GATE, 0, 0);
    setup_interrupt_gate(0x80, trap, TRAP_64_GATE, 3, 1);

    load_idt();
}
