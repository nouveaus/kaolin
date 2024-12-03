[org 0x7c00]
[bits 16]

KERNEL_ENTRY equ 0x2000
KERNEL_SECTORS equ 1

%include "stage0.asm"

%include "stage1.asm"
