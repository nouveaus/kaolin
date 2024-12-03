[bits 16]

; bx = address of string
print:
	pusha

	mov	ah,	0x0E

.print_loop:
	mov	al,	[bx]
	test	al,	al

	je	.print_loop_end

	int	0x10
	inc	bx

	jmp	.print_loop

.print_loop_end:
	popa
	ret

; dh = amount of sectors to read
; bx = offset within the segment (es reg)
disk_load:
	pusha

	mov	ah,	0x02
	mov	al,	dh
	mov	ch,	0x00
	mov	dh,	0x00

	int	0x13

	jc	.disk_err

	popa
	ret

.disk_err:
	mov	bx,	.disk_err_msg
	call	print
	hlt

.disk_err_msg: db "disk_load error!", 0

