#include "../../../io.h"
#include "../../../test.h"
#include "../usermode/usermode.h"
#include "../serial/serial.h"

// ! THIS NEEDS TO BE THE LAST TEST !
static void usermode_function(void) {
    outw(0xf4, 0x10);
    while(1);
}

static void usermode_test(void) {
    enter_usermode(usermode_function);
}

TEST("Enter usermode", usermode_test)
