[bits 32]


load_kernel:
	call	KERNEL_OFFSET
	jmp $

; hopefully we never hit this point since the kernel should be always running from now on
; but we can have something to print that kernel hung and died
