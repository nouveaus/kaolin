[bits 32]

cpuid_avaliability:
	; check if cpuid is even supported by the processor
	; halts if cpuid isn't supported, or if apic chip doesn't exist
	pushfd
	pushfd
	xor	dword [esp], 0x00200000		; id bit flip
	popfd
	pushfd

	pop	eax
	xor	eax,	[esp]
	popfd
	and	eax,	0x00200000

	cmp	eax,	0x0
	je	.cpuid_unavaliable
	ret

.cpuid_unavaliable:
	mov	ebx,	.cpuid_unavaliable_msg
	call	print_vga
	jmp	$

.cpuid_unavaliable_msg db "cpuid is not supported on this chipset", 0

