set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/src/kernel.ld")

add_executable(kernel src/kernel_main.c src/vga.c src/io.c src/serial.c)

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
