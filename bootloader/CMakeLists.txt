enable_language(ASM_NASM)

set(SRC "src/stage0.asm")
set(OUTPUT "stage0.bin")
set(DEPFILE "${OUTPUT}.d")

add_custom_command(
        OUTPUT "${OUTPUT}" "${DEPFILE}"
        COMMAND "${CMAKE_ASM_NASM_COMPILER}" ${CMAKE_ASM_NASM_FLAGS} -f bin -MD "${DEPFILE}"
                "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}" -o "${OUTPUT}"
        DEPENDS "${SRC}"
        DEPFILE "${DEPFILE}"
        COMMENT "Assemble ${SRC} to ${OUTPUT}"
)

add_custom_target(bootloader ALL DEPENDS "${OUTPUT}")
