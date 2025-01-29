ENTRY(kernel_main)

SECTIONS {
    . = 0xD000;

    .text.kernel_main : {
    	*(.text.kernel_main)
    }

    .test_info : {
        test_info_start = .;
        KEEP(*(.test_info));
        test_info_end = .;
    }
}
