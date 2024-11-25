[org 0x7c00]
[bits 16]

; Initialise the stack

	mov bp, 0x7c00
	mov sp, bp


	mov bx, success_stack_msg
	call print

	mov bx, 0x7e00
	mov dh, 1
	call disk_load

	call enable_a20

	mov bx, success_a20_msg
	call print

	call init_vga

	call switch_to_pm

	jmp CODE_SEG:init_pm

success_stack_msg: db "Stack is initialised", 13, 10, 0
success_a20_msg: db "a20 successful", 30, 10, 0

; Imports for sector 1

%include "io.asm"

times 510-($-$$) db 0
dw 0xaa55

; SECOND SEGMENT
second_segement_start:

success_disk_load:
	pusha

	mov bx, .success_disk_load_msg
	call print

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
	
