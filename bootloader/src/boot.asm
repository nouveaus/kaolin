[org 0x7c00]
[bits 16]

KERNEL_OFFSET equ 0x2000

%include "stage0.asm"

%include "stage1.asm"
	
