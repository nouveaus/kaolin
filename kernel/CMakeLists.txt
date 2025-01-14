set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel.ld")

set(COMPILE_OPTIONS
        -ffreestanding          # Not hosted
        -mcmodel=large
        -mno-red-zone           # Required for x86_64
        -Os
)

add_library(drivers OBJECT
        src/arch/x86_64/drivers/keyboard/keyboard.c
        src/arch/x86_64/drivers/timer/timer.c
        src/arch/x86_64/klib/trap.c
)

target_compile_options(drivers PRIVATE
        ${COMPILE_OPTIONS}
        -mgeneral-regs-only
)

add_executable(kernel
        src/arch/x86_64/acpi/acpi.c
        src/arch/x86_64/apic/ioapic.c
        src/arch/x86_64/apic/lapic.c
        src/arch/x86_64/cpu/cpuid.c
        src/arch/x86_64/cpu/idt.c
        src/arch/x86_64/cpu/msr.c
        src/arch/x86_64/io/io.c
        src/arch/x86_64/memory/memmap.c
        src/arch/x86_64/memory/paging.c
        src/arch/x86_64/klib/klib.c
        src/arch/x86_64/serial/serial.c
        src/arch/x86_64/vga/vga.c
        src/kernel_main.c
        src/io.c
        $<TARGET_OBJECTS:drivers>
)


set_target_properties(kernel PROPERTIES
        LINKER_LANGUAGE C
        PREFIX ""               # No lib prefix
        SUFFIX ""               # Raw code file
)

target_compile_options(kernel PRIVATE
        ${COMPILE_OPTIONS}
)

target_link_options(kernel PRIVATE
        -nostdlib               # Not hosted
        -T "${LINKER_SCRIPT}"   # Linker script (relocations, etc.)
        -fno-lto                # LTO causes ld segfaults at the moment
)

# Place a dependency on the linker script
set_target_properties(kernel PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
