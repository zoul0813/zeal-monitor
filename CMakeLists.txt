cmake_minimum_required(VERSION 3.16)

if(NOT EXISTS "$ENV{ZOS_PATH}/cmake/zos_init.cmake")
    message(FATAL_ERROR "Set ZOS_PATH to a Zeal OS checkout with the CMake toolchain, or build with zde cmake.")
endif()

set(ZOS_TOOLCHAIN sdcc)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
include("$ENV{ZOS_PATH}/cmake/zos_init.cmake")

project(zeal_monitor LANGUAGES C ASM)

add_executable(mon src/main.c src/utils.c)
target_compile_options(mon PRIVATE --nostdlib)
zos_add_outputs(mon)

# The monitor's sample at 0x8000 is a separate raw z88dk program.
find_program(Z88DK_ASSEMBLER NAMES z88dk-z80asm)
if(NOT Z88DK_ASSEMBLER)
    message(FATAL_ERROR "Install z88dk-z80asm to build the monitor sample.")
endif()
set(example "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/a.out")
add_custom_command(
    OUTPUT "${example}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    COMMAND "${Z88DK_ASSEMBLER}" -b
        "-I$ENV{ZOS_PATH}/sdk/z88dk-z80asm/"
        "-o${example}" "${CMAKE_CURRENT_SOURCE_DIR}/src/example.asm"
    DEPENDS src/example.asm "$ENV{ZOS_PATH}/sdk/z88dk-z80asm/zos_sys.asm"
    VERBATIM
)
add_custom_target(monitor_example ALL DEPENDS "${example}")
