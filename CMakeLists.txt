cmake_minimum_required(VERSION 3.28)
project(kaolin)

add_subdirectory(bootloader)
add_subdirectory(kernel)
add_subdirectory(tools)

add_custom_command(
        OUTPUT "os.img"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/bind_kernel.py
                --bootloader "bootloader/boot.bin"
                --kernel $<TARGET_FILE:kernel>
                --entry 0xD000
                --output "os.img"
                --cc ${CMAKE_C_COMPILER}
                --ls ${CMAKE_CURRENT_SOURCE_DIR}/kernel/src/x86_64.ld
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/bind_kernel.py
                "bootloader/boot.bin"
                $<TARGET_FILE:kernel>
                ${CMAKE_CURRENT_SOURCE_DIR}/kernel/src/x86_64.ld
        COMMENT "Link bootloader with kernel image"
)

add_custom_target(os ALL DEPENDS "os.img")
add_dependencies(os bootloader kernel)

add_custom_target(run
        COMMAND qemu-system-x86_64
                -cpu qemu64
                -drive file=os.img,format=raw
                -serial stdio
                -device isa-debug-exit,iobase=0xf4,iosize=0x04
                -d int
                -D qemu.log
        DEPENDS os)
