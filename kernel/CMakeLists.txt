set(SRC src/kernel_main.c)
set(OUTPUT kernel.o)
set(KERNEL_BIN kernel.bin)
set(CMAKE_C_COMPILER "i686-elf-gcc")
set(CMAKE_CXX_LINK_EXECUTABLE "i686-elf-ld")

# build kernel.o
add_library(${OUTPUT} OBJECT ${SRC})
target_compile_options(${OUTPUT} PRIVATE -ffreestanding -c)

# link to kernel.bin
add_custom_command(
    OUTPUT "${KERNEL_BIN}"
    COMMAND "${CMAKE_CXX_LINK_EXECUTABLE}" -Ttext 0x2000 --oformat binary
    DEPENDS "${SRC}"
    COMMENT "link ${SRC} to ${KERNEL_BIN}"
)
add_custom_target(link_kernel ALL DEPENDS "${KERNEL_BIN}")

#[[
build/os.bin: build/bootloader.bin build/kernel.bin
	cat $^ > $@
#]]
