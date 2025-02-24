#include "pci.h"

#include <stddef.h>

#include "../../klib/klib.h"
#include "../../serial/serial.h"

#include "../../../../io.h"

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA    0xCFC

static struct common_header *devices = NULL;
static size_t i = 0;

// todo: support PCIE once I figure out a way
// todo: to enable pcie in qemu

// ! THIS ASSUMES WE ARE USING MECHANISM 1
// ! MECHANISM 2 is deprecated

// See: https://wiki.osdev.org/PCI
uint32_t pci_config_readl(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address = 0;
    address |= bus << 16;
    address |= slot << 11;
    address |= func << 8;
    address |= offset & 0xFC;
    address |= 1 << 31;
    outl(CONFIG_ADDRESS, address);
    return inl(CONFIG_DATA);
}

struct common_header pci_get_config(uint8_t bus, uint8_t slot, uint8_t func) {
    struct common_header header = {0};

    // abuses little endian
    ((uint32_t *) &header)[offsetof(struct common_header, vendor_ID) / sizeof(uint32_t)] =
            pci_config_readl(bus, slot, func, 0x0);
    ((uint32_t *) &header)[offsetof(struct common_header, command) / sizeof(uint32_t)] =
            pci_config_readl(bus, slot, func, 0x4);
    ((uint32_t *) &header)[offsetof(struct common_header, revision_ID) / sizeof(uint32_t)] =
            pci_config_readl(bus, slot, func, 0x8);
    ((uint32_t *) &header)[offsetof(struct common_header, cache_line_size) / sizeof(uint32_t)] =
            pci_config_readl(bus, slot, func, 0xC);

    return header;
}

void pci_check_slot(uint8_t bus, uint8_t slot) {
    // todo: expand upon this
    for (size_t func = 0; func < 8; func++) {
        struct common_header header = pci_get_config(bus, slot, func);
        if (header.vendor_ID == 0xFFFF) return;
        if ((header.header_type & 0x80) == 0)
            if (func != 0) continue;

        krintf("Found device at bus %d slot %d function %d\n"
               "Vendor ID: %d, Device ID: %d\n",
               bus, slot, func,
               header.vendor_ID, header.device_ID);
        devices[i++] = header;
    }
}

void pci_scan(void) {
    // bruteforce method - hopefully we dont have more than 32 devices connected
    // todo: have a method to figure out how many devices are connected
    devices = kmalloc(32 * sizeof(struct common_header));
    for (size_t bus = 0; bus < 256; bus++) {
        for (size_t slot = 0; slot < 32; slot++) {
            pci_check_slot(bus, slot);
        }
    }
    i = 0;
}
