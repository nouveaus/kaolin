#include "usermode.h"

#include "../memory/paging.h"

static struct gdt gdt = {0};

struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_descriptor = {
        .limit = sizeof(gdt) - 1,
        .base = (uint64_t) &gdt,
};

static struct tss_entry tss = {0};

#define STACK_SIZE 4096

static uint8_t ist1_stack[STACK_SIZE] __attribute__((aligned(16)));
static uint8_t ist2_stack[STACK_SIZE] __attribute__((aligned(16)));
static uint8_t rsp0_stack[STACK_SIZE] __attribute__((aligned(16)));

void enter_usermode(void *function_address) {

    struct gdt_entry *ring3_kcode = &gdt.kernel_code;
    struct gdt_entry *ring3_kdata = &gdt.kernel_data;
    ring3_kcode->read_write = 1;
    ring3_kcode->code_data_segment = 1;
    ring3_kcode->present = 1;
    ring3_kcode->long_mode = 1;
    ring3_kcode->code = 1;

    *ring3_kdata = *ring3_kcode;
    ring3_kdata->code = 0;
    ring3_kdata->long_mode = 0;

    struct gdt_entry *ring3_code = &gdt.user_code;

    struct gdt_entry *ring3_data = &gdt.user_data;

    // ! EVERYTHING NOT SET IS ZERO !
    ring3_code->limit_low = 0xFFFF;

    ring3_code->read_write = 1;
    ring3_code->code = 1;
    ring3_code->code_data_segment = 1;
    // ring 3
    ring3_code->DPL = 3;
    ring3_code->present = 1;
    ring3_code->limit_high = 0xF;
    ring3_code->available = 1;
    ring3_code->long_mode = 1;
    ring3_code->big = 1;
    ring3_code->gran = 1;

    // data is basically the same besides the code bit
    *ring3_data = *ring3_code;
    ring3_data->code = 0;
    // long mode not used as attribute
    ring3_data->long_mode = 0;

    uint64_t base = (uint64_t) &tss;
    uint32_t limit = sizeof(tss);

    // tss descriptor
    struct gdt_system_entry *ring3_tss = &gdt.tss;
    ring3_tss->base_low = base & 0xFFFFFF;
    ring3_tss->limit_low = limit & 0xFFFFFF;

    ring3_tss->accessed = 1;
    ring3_tss->present = 1;

    // code bit indicates whether its 32 bit or 16 bit
    ring3_tss->code = 1;

    // tss descriptor is ring 0
    // tss available flag = 0

    ring3_tss->base_high = (base >> 27) & 0xFF;
    ring3_tss->limit_high = (limit >> 27) & 0xF;

    tss.ist1 = (uint64_t) &ist1_stack[STACK_SIZE - 1];// this one is for traps
    tss.ist2 = (uint64_t) &ist2_stack[STACK_SIZE - 1];// for double faults
    tss.rsp0 = (uint64_t) &rsp0_stack[STACK_SIZE - 1];// for kernel stack


    // ring 3 time
    asm volatile("lgdt %0\n" ::"m"(gdt_descriptor));

    // flush tss
    asm volatile(
            "mov $40, %%ax\n"
            "ltr %%ax" ::: "ax");

    asm volatile(
            // not too sure if this is allowed for x86_64
            "mov $0x23, %%ax\n"
            "mov %%ax, %%ds\n"
            "mov %%ax, %%es\n" ::);

    asm volatile(
            "mov %0, %%rax\n"// get the stack

            "push $0x23\n"// 0x20 | 3 (user data selector)
            "push %%rax\n"// stack address

            // pushfq is sufficient but just in case
            "push $0x202\n"// rflag (interrupt enable flag on)

            "push $0x1b\n"// 0x18 | 3 (user code selector)
            "push %1\n"   // function
            "iretq\n" ::"r"((uint64_t) &rsp0_stack[STACK_SIZE - 1]),
            "r"((uint64_t) function_address)
            : "rax");

    __builtin_unreachable();
}
