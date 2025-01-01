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

; si = address of disk address packet
; automatically loads to drive 0
disk_load_lba:
	pusha

	mov	ah, 0x42
	mov	dl, 0x80
	mov	si, dap
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

;	magic number for EDD heck
	mov	ah, 0x41
	mov	bx, 0x55AA
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

; Disk address packet
dap:
	; size of DAP
	db	0x10
	; unused, 0
	db	0
dap_sectors:
	; sectors to read. some BIOSes are limited to a single signed byte (127)
	dw	0
dap_dest:
	; destination address
	dd	0
dap_lba:
	; little-endian start of LBA address to read. 0-indexed
	dd	0
	dd	0
