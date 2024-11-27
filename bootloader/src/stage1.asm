[bits 16]

; SECOND SEGMENT
second_segement_start:

success_disk_load:
	pusha

	mov	bx, .success_disk_load_msg
	call	print

	popa
	ret

.success_disk_load_msg: db "Disk load successful", 13, 10, 0

; Imports for sector 2 (16 bits)

%include "a20.asm"
%include "pm.asm"

[bits 32]

; Move to own file later
init_pm:

	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

; New stack
	mov ebp, 0x90000
	mov esp, ebp

; protected mode is on from here onwards

	mov ebx, .success_init_pm_msg
	call print_vga

	hlt

.success_init_pm_msg db "init_pm successful", 0

; Imports for sector 2 (32 bits)

%include "io32.asm"

times 510-($-second_segement_start) db 0
