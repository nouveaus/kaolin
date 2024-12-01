add_executable(kernel src/kernel_main.c)

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
        -Ttext=0x2000           # Code is relocated
        -Wl,--oformat=binary    # Raw code file
)
