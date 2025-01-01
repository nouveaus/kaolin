cmake_minimum_required(VERSION 3.28)
project(kaolin)

add_subdirectory(bootloader)
add_subdirectory(kernel)
add_subdirectory(tools)

add_custom_command(
        OUTPUT "os.img"
        COMMAND cat "bootloader/boot.bin" $<TARGET_FILE:kernel> > "os.img"
        DEPENDS "bootloader/boot.bin" $<TARGET_FILE:kernel>
        COMMENT "Link bootloader with kernel image"
)

add_custom_target(os ALL DEPENDS "os.img")
add_dependencies(os bootloader kernel)

add_custom_target(run COMMAND qemu-system-x86_64 -cpu qemu64 -drive file=os.img,format=raw DEPENDS os)
