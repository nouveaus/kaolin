#include "../../../io.h"
#include "../../../test.h"
#include "../serial/serial.h"
#include "../usermode/usermode.h"

// ! THIS NEEDS TO BE THE LAST TEST !
static void usermode_function(void) {
    char *finish = "Usermode test successful\n";

    asm volatile(
            "mov %0, %%rax\n"
            "mov %1, %%rdi\n"
            "int %2"
            : : "r"((unsigned long) 0), "r"(finish), "i"(0x80) : "memory");
    outw(0xf4, 0x10);
    while(1);
}

static void usermode_test(void) {
    enter_usermode(usermode_function);
}

TEST("Enter usermode", usermode_test)
