#include "acpi.h"

#include <stddef.h>
#include <stdbool.h>

#include "vga.h"
#include "io.h"

/*
// Compares two strings
// * for now its static, it will be moved to its own library in future
static int strcmp(char *s1, char *s2) {
    for (;*s1 == *s2 && *s1 && *s2; *s1++, *s2++);
    return *s1 - *s2;
}
*/

#define BIOS_MEMORY_BEGIN 0x0E0000
#define BIOS_MEMORY_END 0xFFFFF

#define RSDP_SIGNATURE "RSD PTR "

static bool cmp_signature(char *s1, char *s2) {
    for (int i = 0; s2[i]; i++) {
        if (s1[i] != s2[i]) return false;
    }
    return true;
}

static struct rsdp *rsdp = NULL;

bool rsdp_find(void) {
    // Refer to 5.2.5.1 and 5.2.5.3 from ACPI Specification
    char *addr = (char *)BIOS_MEMORY_BEGIN;
    for (; addr < (char *)BIOS_MEMORY_END && !cmp_signature(addr, RSDP_SIGNATURE); addr += 16);
    rsdp = (struct rsdp*)addr;
    return addr < (char *)BIOS_MEMORY_END;
}

void rsdp_print_signature(void) {
    for (size_t i = 0; i < 8; i++) {
        vga_putchar(rsdp->signature[i]);
    }
    vga_putchar('\n');
}

bool rsdp_verify(void) {
    // Refer to 5.2.5.3 from ACPI Specification
    uint8_t *addr = (int8_t *)rsdp;
    // ! IMPORTANT sum needs to be unsigned 8 bits
    uint8_t sum = 0;
    for (size_t i = 0; i < 20; i++) {
        sum += addr[i];
    }
    return !sum;
}

int rsdp_get_revision(void) {
    return rsdp->revision;
}

static inline struct rsdt *rsdt_get(void) {
    return (struct rsdt *)rsdp->rsdt_address;
}

bool rsdt_verify(void) {
    struct rsdt *rsdt = rsdt_get();
    uint8_t sum = 0;
    for (size_t i = 0; i < rsdt->length; i++) {
        sum += ((uint8_t *)rsdt)[i];
    }
    return !sum;
}

static size_t rsdt_get_entry_count(void) {
    struct rsdt *rsdt = rsdt_get();
    return (rsdt->length - sizeof(struct rsdt)) / sizeof(uint32_t);
}

#define MADT_SIGNATURE "APIC"

static struct madt *madt = NULL;

bool madt_find(void) {
    size_t length = rsdt_get_entry_count();
    struct rsdt *rsdt = rsdt_get();
    for (size_t i = 0; i < length; i++) {
        struct description_header *description_header = (struct description_header*)rsdt->entry[i];
        if (cmp_signature(description_header->signature, MADT_SIGNATURE)) {
            madt = (struct madt*)rsdt->entry[i];
            return true;
        }
    }
    return false;
}

bool madt_verify(void) {
    uint8_t sum = 0;
    uint8_t *addr = (uint8_t*)madt;
    for (size_t i = 0; i < madt->length; i++) {
        sum += addr[i];
    }
    return !sum;
}

uint32_t madt_get_lapic_address(void) {
    return madt->local_interrupt_controller_address;
}

static size_t madt_get_entry_count(void) {
    return madt->length - sizeof(struct madt);
}

static uint32_t ioapic_address = 0;

size_t ioapic_get_entry_count(void) {
    size_t length = madt_get_entry_count();
    // all entries are after madt
    struct madt_entry *madt_entry = (struct madt_entry*)(madt + sizeof(struct madt));
    // they vary in different types tho
    size_t count = 0;
    for (size_t i = 0; i < length - sizeof(struct madt); i++, madt_entry += madt_entry->length) {
        if (madt_entry->type != 0) krintf("type: %d, length: %d\n", madt_entry->type, madt_entry->length);
        if (madt_entry->type == 1) {
            // we cast it 
            struct madt_entry_apicio *madt_entry_apicio = (struct madt_entry_apicio*)madt_entry;
            if (!ioapic_address) ioapic_address = madt_entry_apicio->ioapic_address;
        }
    }
    return count;
}

uint32_t get_first_ioapic_address(void) { return ioapic_address; }