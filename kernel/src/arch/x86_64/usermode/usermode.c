#include "usermode.h"

static struct gdt gdt = {0};
static struct tss_entry tss = {0};

static uint8_t ist1_stack[4096];
static uint8_t ist2_stack[4096];
static uint8_t rsp0_stack[4096];

void enter_usermode(void *function_address) {
    struct gdt_entry *ring3_code = &gdt.code;
    struct gdt_entry *ring3_data = &gdt.data;

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

    uint64_t base = (uint64_t) &tss;
    uint32_t limit = sizeof tss;

    // tss descriptor
    struct gdt_system_entry *ring3_tss = &gdt.tss;
    ring3_tss->limit_low = limit;
    ring3_tss->base_low = base & 0xFFFFFF;
    ring3_tss->accessed = 1;

    // code bit indicates whether its 32 bit or 16 bit
    ring3_tss->code = 1;
    // tss descriptor is ring 0
    // tss available flag = 0
    ring3_tss->base_high = (base >> 27) & 0xFF;
    ring3_tss->limit_low = limit & 0xFFFFFFF;
    ring3_tss->limit_high = (limit >> 27) & 0xf;

    tss.ist1 = (uint64_t) &ist1_stack[4095];// this one is for traps
    tss.ist2 = (uint64_t) &ist2_stack[4095];// for double faults
    tss.rsp0 = (uint64_t) &rsp0_stack[4095];// for kernel stack

    struct {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed)) gdt_descriptor = { 0 };

    gdt_descriptor.limit = sizeof(gdt) - 1;
    gdt_descriptor.base = (uint64_t)&gdt;

    // ring 3 time
    asm volatile(
            "lgdt %0\n"

            // not too sure if this is allowed for x86_64
           // "mov ax, $0x23\n"
           // "mov ds, ax\n"
           // "mov es, ax\n"
           // "mov fs, ax\n"
           // "mov gs, ax\n"

            "mov %%rax, %%rsp\n"// get the stack
            "push $0x23\n"      // 0x23 | 3 (data selector)
            "push %%rax\n"      // stack address
            // pushf is sufficient but just in case
            "push $0x202\n" // rflag (interrupt enable flag on)
            "push $0x1B\n"// 0x18 | 3 (code selector)
            "push %1\n"   // function
            "iretq\n" ::"m"(gdt), "m"(function_address) );
}

// osdev said I needed this
void tss_set_kernel_stack(uint64_t stack) {
    tss.rsp0 = stack;
}