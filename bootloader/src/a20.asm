[bits 16]

enable_a20:
	pusha

	mov	ax,	0x2401
	int	0x15

;	fallback to keyboard if fails
	jc	.enable_a20_old

.enable_a20_end:

	call	.test_a20

	popa
	ret

.enable_a20_old:
	cli

	call	.wait_a20
	mov	al,	0xAD
	out	0x64,	al

	call	.wait_a20
	in	al,	0xD0
	out	0x64,	al

	call	.wait_2_a20
	in	al,	0x60

;	osdev had a push eax here
	call	.wait_a20
	mov	al,	0xD1
	out	0x64,	al

	call	.wait_a20
	or	al,	2
	out	0x60,	al

	call	.wait_a20
	mov	al,	0xAE
	out	0x64,	al

	call	.wait_a20
	sti
		
	jmp	.enable_a20_end


.wait_a20:
	in	al,	0x64
	test	al,	2
	jnz	.wait_a20
	ret

.wait_2_a20:
	in	al,	0x64
	test		al,	2
	jnz	.wait_2_a20

.test_a20:
	pusha

	mov	ax,	0x0000
	mov	es,	ax
	mov	byte	[es:0x000000],	0x55
	mov	byte	[es:0x001000],	0xAA

	cmp	byte	[es:0x000000],	0x55
	jne	.test_a20_err

	cmp	byte	[es:0x001000],	0xAA
	jne	.test_a20_err

	popa
	ret

.test_a20_err:
	mov	bx,	.test_a20_err_msg
	call	print
	hlt

.test_a20_err_msg:	db "a20 test error!", 0
