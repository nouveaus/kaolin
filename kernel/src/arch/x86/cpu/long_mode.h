#pragma once

/*
0x8000
the address straight after the other sector
of the bootloader
we load before from 0x7e00
*/
#define PAGING_TABLE_START_ADDRESS 0x8000

#define PML4T_START_ADDRESS PAGING_TABLE_START_ADDRESS
#define PDPT_START_ADDRESS PML4T_START_ADDRESS + 0x1000
#define DPT_START_ADDRESS PDPT_START_ADDRESS + 0x1000
#define PT_START_ADDRESS DPT_START_ADDRESS + 0x1000

bool long_mode_is_supported(void);
void enable_long_mode(void);
void setup_paging(void);
