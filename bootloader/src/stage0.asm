[bits 16]
;	Initialise the stack

	mov	bp,	0x7c00
	mov	sp,	bp


	mov	bx,	success_stack_msg
	call	print

	mov	bx,	0x7e00
	mov	dh,	1
	call	disk_load

	call	enable_a20

	mov	bx,	success_a20_msg
	call	print

	call	init_vga

	call	switch_to_pm

	jmp	CODE_SEG:init_pm

success_stack_msg: db "Stack is initialised", 13, 10, 0
success_a20_msg: db "a20 successful", 30, 10, 0

; Imports for sector 1

%include "io.asm"

times 510-($-$$) db 0
dw 0xAA55
