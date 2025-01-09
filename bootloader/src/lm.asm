[bits 16]

PAGE_PRESENT    equ (1 << 0)
PAGE_WRITE      equ (1 << 1)

; check if it collides with anything later on
paging_table_buffer equ 0x9000

gdt:
.gdt_start:
	; NULL descriptor
	dq	0x0
; from https://wiki.osdev.org/Entering_Long_Mode_Directly
.gdt_code:
	; Segment limit Low (16 bits, 0-15) (ignored)
	dw	0x0
	; Base Address Low (16 bits, 16-31) (ignored)
	dw	0x0
	; Base Address Mid (8 bits, 24-31) (ignored)
	db	0x0
	; access (8 bits, 8-15)
	db	10011010b
	; Segment Limit High (4 lower bits, 16-19) (ignored)
	; flags (4 higher bits, 20-23)
	db	00100000b
	; Segment limit High (4 bits, 16-19) (ignored)
	db	0x0

.gdt_data:
	; Segment limit Low (16 bits, 0-15) (ignored)
	dw	0x0
	; Base Address Low (16 bits, 16-31) (ignored)
	dw	0x0
	; Base Address Mid (ignored)
	db	0x0
	; access
	db	10010010b

	; Segment Limit High (Ignored) (4 lower bits)
	; Flags (4 higher bits)
	db	0x0
	; Base Address High (16 bits) (ignored)
	dw	0x0	
	
	; align on 4 byte boundary
	align	4
	dw	0
.gdt_end:
gdt_descriptor:
	; 16 bit size
	; subtract 1 for padding
	dw	gdt.gdt_end - gdt.gdt_start - 1
	dd	gdt.gdt_start

; pusha and popa used to be here
init_vga:
	pusha
	mov	ax,	0x3
	int	0x10
	popa
	ret

switch_to_lm:
	; todo: move paging to its own file
	pusha
	
	; zero out 16kib buffer for 4 page tables
	push	di
	mov	ecx, 	0x1000
	xor	eax, 	eax
	cld
	rep	stosd
	pop	di

	cli

	; point the page map level 4 [0]
	; to the page directory pointer table

	; address of page directory pointer table
	lea	eax, 	[di + 0x1000]
	or	eax, 	PAGE_PRESENT | PAGE_WRITE
	mov	[di], eax

	; point the page directory pointer table [0]
	; to the page directory

	; address of page directory
	lea	eax, 	[di + 0x2000]
	or	eax, 	PAGE_PRESENT | PAGE_WRITE
	mov	[di + 0x1000], eax

	; point the page directory [0]
	; to the page table

	; address of page table
	lea	eax, 	[di + 0x3000]
	or	eax, 	PAGE_PRESENT | PAGE_WRITE
	mov	[di + 0x2000], eax

	; initialise variables before build
	push	di
	lea	di, 	[di + 0x3000]
	mov	eax, 	PAGE_PRESENT | PAGE_WRITE

	; builds page table
	; it points each entry to a address
.loop_page_table:
	mov	[di], eax
	add 	eax,	0x1000
	; each entry is 64 bits
	add	di,	8
	; stop when we finish 2MiB
	cmp	eax,	0x200000
	jb	.loop_page_table
	pop	di

	; set pae and pge bit
	mov	eax, 	10100000b
	mov	cr4,	eax

	; point cr3 to PML4
	mov	edx,	edi
	mov	cr3,	edx

	; read from efer msr
	mov	ecx,	0xC0000080
	rdmsr

	; set lme bit
	or	eax,	0x00000100
	wrmsr

	mov	eax,	cr0
	; enables paging and protection
	or	eax,	0x80000001
	mov	cr0,	eax

	lgdt	[gdt_descriptor]

	popa
	ret

CODE_SEG equ gdt.gdt_code - gdt.gdt_start
DATA_SEG equ gdt.gdt_data - gdt.gdt_start
