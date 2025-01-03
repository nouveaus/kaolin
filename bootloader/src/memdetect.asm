[bits 32]

; https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15,_EAX_=_0xE820
mmap_buf	equ	0x8000
detect_memory:
	pusha
	mov	eax,	0xE820
	mov	ebx,	0
	mov	ecx,	24		; sizeof es:di buffer
	mov	edx,	0x534D4150	; place 'SMAP' into EDX
	mov	di,	0x8000		; setup ES:DI buffer
	mov	[es:di + 20],	dword 1
	int	0x15

	jc	.detect_memory_err
	cmp	eax,	edx		; eax is set to SMAP on success
	jne	.detect_memory_err

	popa
	ret

.detect_memory_err:
	mov	ebx,	.detect_memory_err_msg
	call	print
	hlt

.detect_memory_err_msg: db "Detecting upper memory fail!", 0



