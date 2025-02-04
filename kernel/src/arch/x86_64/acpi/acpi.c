#include "acpi.h"

#include <stdbool.h>
#include <stddef.h>

#include "../../../io.h"
#include "../klib/klib.h"
#include "../memory/paging.h"

#define BIOS_MEMORY_BEGIN 0x0E0000
#define BIOS_MEMORY_END   0xFFFFF

#define RSDP_SIGNATURE "RSD PTR "

// TODO: Implement case where acpi version >= 2.0
// TODO: needs xsdt

static bool cmp_signature(char *s1, char *s2) {
    for (int i = 0; s2[i]; i++) {
        if (s1[i] != s2[i]) return false;
    }
    return true;
}

static struct rsdp *rsdp = NULL;

bool rsdp_find(void) {
    // Refer to 5.2.5.1 and 5.2.5.3 from ACPI Specification
    char *addr = (char *) BIOS_MEMORY_BEGIN;
    for (;
         addr < (char *) BIOS_MEMORY_END && memcmp(addr, RSDP_SIGNATURE, 8);
         addr += 16);
    rsdp = (struct rsdp *) addr;
    return addr < (char *) BIOS_MEMORY_END;
}

void rsdp_print_signature(void) {
    for (size_t i = 0; i < 8; i++) {
        putc(rsdp->signature[i]);
    }
    putc('\n');
}

// No need to map rsdp I think cause its in bios (already mapped)
bool rsdp_verify(void) {
    // Refer to 5.2.5.3 from ACPI Specification
    uint8_t *addr = (int8_t *) rsdp;
    // ! IMPORTANT sum needs to be unsigned 8 bits
    uint8_t sum = 0;
    for (size_t i = 0; i < 20; i++) {
        sum += addr[i];
    }
    return !sum;
}

uint8_t rsdp_get_revision(void) {
    return rsdp->revision;
}

static struct rsdt *rsdt_get(void) {
    return (struct rsdt *) (uint64_t) rsdp->rsdt_address;
}

bool rsdt_map(void) {
    uint64_t address = (uint64_t) rsdp->rsdt_address;
    map_page(KERNEL_MAPPING_ADDRESS | address, address, PAGE_PRESENT);
    return verify_mapping(KERNEL_MAPPING_ADDRESS | address);
}

bool rsdt_verify(void) {
    struct rsdt *rsdt =
            (struct rsdt *) (KERNEL_MAPPING_ADDRESS | rsdp->rsdt_address);
    uint8_t sum = 0;
    for (size_t i = 0; i < rsdt->length; i++) {
        sum += ((uint8_t *) rsdt)[i];
    }
    return !sum;
}

static size_t rsdt_get_entry_count(void) {
    struct rsdt *rsdt =
            (struct rsdt *) (KERNEL_MAPPING_ADDRESS | rsdp->rsdt_address);
    return (rsdt->length - sizeof(struct rsdt)) / sizeof(uint32_t);
}

#define MADT_SIGNATURE "APIC"

static struct madt *madt = NULL;

bool madt_find(void) {
    size_t length = rsdt_get_entry_count();
    struct rsdt *rsdt =
            (struct rsdt *) (KERNEL_MAPPING_ADDRESS | rsdp->rsdt_address);

    for (size_t i = 0; i < length; i++) {
        map_page(KERNEL_MAPPING_ADDRESS | (uint64_t) rsdt->entry[i],
                 (uint64_t) rsdt->entry[i], PAGE_PRESENT);

        struct description_header *description_header =
                (struct description_header *) (KERNEL_MAPPING_ADDRESS |
                                               (uint64_t) rsdt->entry[i]);
        if (!memcmp(description_header->signature, MADT_SIGNATURE, 4)) {
            madt = (struct madt *) (uint64_t) rsdt->entry[i];
            return true;
        }
    }
    return false;
}

bool madt_map(void) {
    uint64_t address = (uint64_t) madt;
    map_page(KERNEL_MAPPING_ADDRESS | address, address, PAGE_PRESENT);
    madt = (struct madt *) (KERNEL_MAPPING_ADDRESS | address);
    return verify_mapping(KERNEL_MAPPING_ADDRESS | address);
}

bool madt_verify(void) {
    uint8_t sum = 0;
    uint8_t *addr = (uint8_t *) madt;
    for (size_t i = 0; i < madt->length; i++) {
        sum += addr[i];
    }
    return !sum;
}

uint32_t madt_get_lapic_address(void) {
    return madt->local_interrupt_controller_address;
}

static struct madt_entry **madt_entries = NULL;
static size_t max_madt_entries = 0;

size_t ioapic_count_entries(void) {
    // all entries are after madt
    struct madt_entry *madt_entry =
            (struct madt_entry *) ((uint8_t *) madt + sizeof(struct madt));
    // madt->length is length of bytes to just add madt with length to get end
    // address

    uint8_t *end = (uint8_t *) madt + madt->length;
    max_madt_entries =
            (end - (uint8_t *) madt_entry) / sizeof(struct madt_entry);
    if (madt_entries) free(madt_entries);
    madt_entries = kmalloc(sizeof(*madt_entries) * max_madt_entries);

    // they vary in different types tho

    for (size_t i = 0; (uint8_t *) madt_entry < end; i++) {
        madt_entries[i] = madt_entry;
        madt_entry =
                (struct madt_entry *) ((uint8_t *) madt_entry + madt_entry->length);
    }
    return max_madt_entries;
}

struct madt_entry **get_madt_entries(void) {
    return madt_entries;
}
