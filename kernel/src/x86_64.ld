ENTRY(kernel_main)
OUTPUT_FORMAT(binary)

SECTIONS {
    . = 0xD000;

    .text.kernel_main : {
    	*(.text.kernel_main)
    }
}
