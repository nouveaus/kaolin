[bits 64]

VIDEO_MEMORY	equ 0xB8000
WHITE_ON_BLACK	equ 0x0F

print_vga:
	push	rax
	push	rdx
	push	rbx
        mov	rdx,	VIDEO_MEMORY

; TODO: Optimise this later (e.g. could use lodsb)
.print_vga_loop:
	mov	al,	[rbx]
	mov	ah,	WHITE_ON_BLACK

	cmp	al,	0
	je	.print_vga_loop_end

	mov	[rdx],	ax

	inc	rbx
	add	rdx,	2

	jmp	.print_vga_loop

.print_vga_loop_end:
	pop	rbx
	pop	rdx
	pop	rax
	ret
