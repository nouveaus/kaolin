[bits 16]
;	Initialise the stack

	mov	bp,	0x7c00
	mov	sp,	bp

	mov	bx,	success_stack_msg
	call	print

	call	lba_check

	; BIOS give us the booting drive index in dl
	; dap_dest = buffer address (where the data will be copied to)
	; dap_sectors = how many sectors to read
	; dap_lba = the first sector you want to read and write
	mov	dword	[dap_dest], 0x7e00
	mov	word	[dap_sectors], 1
	mov	dword	[dap_lba], 0x1
	call	disk_load_lba

	mov	dword	[dap_dest], KERNEL_ENTRY
	mov	word	[dap_sectors], KERNEL_SECTORS
	mov	dword	[dap_lba], 0x2
	call	disk_load_lba

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
