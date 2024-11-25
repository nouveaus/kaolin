[bits 32]

VIDEO_MEMORY equ 0xB8000
WHITE_ON_BLACK equ 0x0F

print_vga:
        pusha
        mov edx, VIDEO_MEMORY

; TODO: Optimise this later (e.g. could use lodsb)
.print_vga_loop:
        mov al, [ebx]
        mov ah, WHITE_ON_BLACK

        cmp al, 0
        je .print_vga_loop_end

        mov [edx], ax

        inc edx,
        add edx, 2

        jmp .print_vga_loop

.print_vga_loop_end:
        popa
        ret