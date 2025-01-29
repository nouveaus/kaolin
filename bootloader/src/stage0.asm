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

	; temporarily load kernel image metadata
	; 0xD000..0xD004 : uintptr_t entry
	; 0xD000..0xD006 : uint16_t sectors
	mov	dword	[dap_dest], 0xD000
	mov	word	[dap_sectors], 1
	mov	dword	[dap_lba], 0x2
	call	disk_load_lba

	mov	eax,	dword [0xD000]
	mov	dword	[dap_dest], eax
	mov	dword	[kernel_entry], eax

	mov	ax,	word [0xD004]
	mov	word	[dap_sectors], ax

	; load actual kernel image
	mov	dword	[dap_lba], 0x3
	call	disk_load_lba

	call	enable_a20
	call	detect_memory
	call	cpuid_avaliability

	call	init_vga

	mov	edi, paging_table_buffer

	call	switch_to_lm


	; breaks here
	jmp	CODE_SEG:boot_kernel

success_stack_msg: db "Stack is initialised", 13, 10, 0
success_a20_msg: db "A20 is initialised", 13, 10, 0

; Imports for sector 1

%include "io.asm"
%include "memdetect.asm"
%include "cpuid.asm"

times 510-($-$$) db 0
dw 0xAA55
