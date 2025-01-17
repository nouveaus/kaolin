#include "../../../io.h"
#include "../../../test.h"
#include "../memory/paging.h"
#include <stdint.h>

static void mmap_test(void) {
    uint64_t test_virtual_address = (KERNEL_MAPPING_ADDRESS | 0xFFFFFFF);
    map_page(test_virtual_address, 0xFFFFFFF, PAGE_PRESENT);

    if (!verify_mapping(test_virtual_address)) {
        puts("Could not page correctly!\n");
        test_fail();
    }

    free_address(test_virtual_address);

    if (verify_mapping(test_virtual_address)) {
        puts("Could not free virtual address correctly!\n");
        test_fail();
    }

    puts("Successfully freed virtual address!\n");
}

TEST("Map and unmap page", mmap_test)
