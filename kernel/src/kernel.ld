/* kernel.ld - A simple linker script for the kernel */

ENTRY(kernel_main)         /* Define the entry point for the kernel */
OUTPUT_FORMAT(binary)      /* Raw code file */

SECTIONS {
    /* Define the memory layout and where sections are placed */

    /* Place the code starting at 0x2000 */
    . = 0x2000; /* Set the starting address for code */

    /* kmain must be absolutely first */
    .text.kernel_main : {
    	*(.text.kernel_main)
    }

    /* Other sections can be placed as needed */
    .text : {
        *(.text)           /* All .text sections go here (functions) */
    }

    .rodata : {
        *(.rodata)         /* All .rodata sections go here (read-only data) */
    }

    .data : {
        *(.data)           /* All .data sections go here (initialized data) */
    }

    .bss : {
        *(.bss)            /* All .bss sections (uninitialized data) go here */
    }
}
