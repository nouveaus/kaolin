#include "cpuid.h"
#include "../../../io.h"

void print_vendor(unsigned int ebx, unsigned int ecx, unsigned int edx) {
    char vendor[14];
    *((unsigned int *) &vendor[0]) = ebx;
    *((unsigned int *) &vendor[4]) = edx;
    *((unsigned int *) &vendor[8]) = ecx;
    vendor[12] = '\n';
    vendor[13] = 0;
    puts(vendor);
}
