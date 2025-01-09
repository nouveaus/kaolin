[bits 16]

; SECOND SEGMENT
second_segement_start:

; Imports for sector 2 (16 bits)
%include "a20.asm"
%include "lm.asm"

[bits 64]

; Move to own file later
boot_kernel:

	mov	ax,	DATA_SEG
	mov	ds,	ax
	mov	ss,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax

; New stack
	mov 	rbp, 	0x70000
	mov 	rsp, 	rbp

; long mode is on from here onwards

	mov	rbx,	.success_init_pm_msg
	call	print_vga

;	struct address_range_descriptor *address_ranges
	lea	rax, [mmap_buf+4]
	push	rax

;	uint32_t address_range_count
	mov	rax, [mmap_buf]
	push	rax 
;	finally enter the kernel in 32-bit protected mode
	hlt
	call	[kernel_entry]

	; we shouldn't have gotten here, disable interrupts and sleep
	cli
	hlt

.success_init_pm_msg db "init_pm successful", 0

kernel_entry:
	dq	0

; Imports for sector 2 (32 bits)

%include "io64.asm"

times 512-($-second_segement_start) db 0
