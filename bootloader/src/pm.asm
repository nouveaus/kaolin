[bits 16]


.gdt_start:
	dd 0x0
	dd 0x0

.gdt_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 11001111b
	db 0x0

.gdt_data:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0

.gdt_end:

gdt_descriptor:
	dw .gdt_end - .gdt_start
	dd .gdt_start

init_vga:
	pusha

	mov ax, 0x3
	int 0x10

	popa
	ret

switch_to_pm:
	pusha

	cli
	lgdt [gdt_descriptor]

	mov   eax, cr0
	or    eax, 0x1
	mov   cr0, eax

	popa
	ret

CODE_SEG equ .gdt_code - .gdt_start
DATA_SEG equ .gdt_data - .gdt_start
