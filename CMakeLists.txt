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
                --entry 0x2000
                --output "os.img"
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/bind_kernel.py
                "bootloader/boot.bin"
                $<TARGET_FILE:kernel>
        COMMENT "Link bootloader with kernel image"
)

add_custom_target(os ALL DEPENDS "os.img")
add_dependencies(os bootloader kernel)

add_custom_target(run COMMAND qemu-system-x86_64 -cpu qemu64 -drive file=os.img,format=raw -serial stdio -no-reboot -d int -D qemu.log DEPENDS os)
