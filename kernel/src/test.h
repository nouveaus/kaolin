#pragma once

#ifndef KAOLIN_TESTS
#    error "test.h was included but tests are not enabled for this build"
#endif

struct test_info {
    char *name;
    void (*test)(void);
};

#define TEST(name, test) \
    struct test_info __##test##_info __attribute__((section(".test_info"), used)) = {name, test};

// Fails the current test
_Noreturn void test_fail(void);
