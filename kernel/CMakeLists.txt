set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel.ld")

add_executable(kernel
        src/arch/x86/acpi/acpi.c
        src/arch/x86/apic/ioapic.c
        src/arch/x86/apic/lapic.c
        src/arch/x86/cpu/cpuid.c
        src/arch/x86/cpu/long_mode.c
        src/arch/x86/cpu/msr.c
        src/arch/x86/cpu/idt.c
        src/arch/x86/cpu/gdt.c
        src/arch/x86/drivers/keyboard/keyboard.c
        src/arch/x86/drivers/timer/timer.c
        src/arch/x86/io/io.c
        src/arch/x86/memory/memmap.c
        src/arch/x86/klib/klib.c
        src/arch/x86/serial/serial.c
        src/arch/x86/vga/vga.c
        src/kernel_main.c
)

set_target_properties(kernel PROPERTIES
        LINKER_LANGUAGE C
        PREFIX ""               # No lib prefix
        SUFFIX ""               # Raw code file
)

target_compile_options(kernel PRIVATE
        -ffreestanding          # Not hosted
)

target_link_options(kernel PRIVATE
        -nostdlib               # Not hosted
        -T "${LINKER_SCRIPT}"   # Linker script (relocations, etc.)
        -fno-lto                # LTO causes ld segfaults at the moment
)

# Place a dependency on the linker script
set_target_properties(kernel PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
