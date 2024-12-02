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
	mov	cl,	0x02

	int	0x13

	jc	.disk_err

	popa
	ret

.disk_err:
	mov	bx,	.disk_err_msg
	call	print
	hlt

.disk_err_msg: db "disk_load error!", 0

; si = address of disk address packet
; automatically loads to drive 0
disk_load_lba:
	pusha

	mov	ah, 0x42
	mov	dl, 0x80
	int	0x13
	; carry bit set if err
	jc	.disk_lba_err

	popa
	ret

.disk_lba_err:
	mov	bx, .disk_lba_err_msg
	call	print
	hlt

.disk_lba_err_msg: db "disk_load_lba error!", 0

lba_check:
	pusha
	xor	ax, ax
	mov	es, ax
;	magic number for EDD heck
	mov	bx, 0x55AA
	mov	ah, 0x41
	mov	dl, 0x80
	int	0x13

	jc	.lba_check_err
	
	cmp	bx, 0xAA55
	jne	.lba_check_err

	popa
	ret

.lba_check_err:
	mov	bx, .lba_check_err_msg
	call	print
	hlt

.lba_check_err_msg: db "lba not supported", 0