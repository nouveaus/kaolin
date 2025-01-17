#include "../test.h"

#include "../arch/x86_64/klib/klib.h"
#include "../io.h"

static void malloc_test(void) {
    int *test = kmalloc(sizeof(int));
    *test = 2;
    krintf("test: %d\n", *test);
    int *test2 = kmalloc(sizeof(int) * 4096);
    puts("test 2\n");
    for (int i = 0; i < 4096; i++) {
        test2[i] = i;
        krintf("%d, ", test2[i]);
    }
    putc('\n');
    free(test);
    free(test2);
}

TEST("Simple heap test", malloc_test)
