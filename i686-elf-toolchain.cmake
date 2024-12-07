# i686-elf-toolchain.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler
set(CROSS_PREFIX /home/suwa/opt/cross/bin/i686-elf-)
set(CMAKE_ADDR2LINE ${CROSS_PREFIX}addr2line)
set(CMAKE_AR ${CROSS_PREFIX}ar)
set(CMAKE_ASM_COMPILER ${CROSS_PREFIX}as)
set(CMAKE_CXX_COMPILER ${CROSS_PREFIX}g++)
set(CMAKE_CXX_COMPILER_AR ${CROSS_PREFIX}ar)
set(CMAKE_CXX_COMPILER_RANLIB ${CROSS_PREFIX}ranlib)
set(CMAKE_C_COMPILER ${CROSS_PREFIX}gcc)
set(CMAKE_C_COMPILER_AR ${CROSS_PREFIX}ar)
set(CMAKE_C_COMPILER_RANLIB ${CROSS_PREFIX}ranlib)
set(CMAKE_LINKER ${CROSS_PREFIX}ld)
set(CMAKE_NM ${CROSS_PREFIX}nm)
set(CMAKE_OBJCOPY ${CROSS_PREFIX}objcopy)
set(CMAKE_OBJDUMP ${CROSS_PREFIX}objdump)
set(CMAKE_RANLIB ${CROSS_PREFIX}ranlib)
set(CMAKE_READELF ${CROSS_PREFIX}readelf)
set(CMAKE_STRIP ${CROSS_PREFIX}strip)

# Prevent CMake from adding standard flags for the host platform
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS "")

# Specify the target architecture
set(CMAKE_C_COMPILER_TARGET i686-elf)
set(CMAKE_CXX_COMPILER_TARGET i686-elf)

# Skip compiler tests
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# target environment on the build host system
set(CMAKE_FIND_ROOT_PATH /home/suwa/opt/cross)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)