[bits 16]

; https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15,_EAX_=_0xE820
; 0x8000
; the address straight after the other sector
; of the bootloader
; we load before from 0x7e00
mmap_buf	equ	0x8000
detect_memory:
	pusha
; 	just in case
	xor 	ax,	ax
	mov	es,	ax
	xor	bp,	bp
	xor	ebx,	ebx		; 0 to indicate first call
;	we need the first byte to be the length so we skip 4 initially
	mov	di,	mmap_buf + 4		; setup ES:DI buffer
;	no need to set ebx, it will be set by the E820 function
.detect_memory_loop:
	mov	eax,	0xE820		; function code
	mov	ecx,	24		; sizeof es:di buffer
	mov	edx,	0x534D4150	; place 'SMAP' into EDX
	; extended attribute for address range descriptor
	; must have bit 0 be 1
	mov	[es:di + 20],	dword 1
	int	0x15

	jc	.detect_memory_err
	cmp	eax,	0x534D4150 ; eax is set to SMAP on success
	jne	.detect_memory_err

	jcxz	.detect_memory_post ; skip 0 length entries
	inc	bp
	add	di,	24
.detect_memory_post:
	test	ebx,	ebx
	jz	.detect_memory_end
	jmp	.detect_memory_loop
.detect_memory_end:
;	count is stored here
	mov	[es:mmap_buf], bp
	popa
	ret

.detect_memory_err:
	mov	ebx,	.detect_memory_err_msg
	call	print
	hlt

.detect_memory_err_msg: db "Detecting upper memory fail!", 0
