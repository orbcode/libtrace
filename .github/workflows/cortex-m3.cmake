set(CMAKE_CROSSCOMPILING 1)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_SYSTEM_NAME Generic)

set(COMPILER_PREFIX arm-none-eabi-)

find_program(CMAKE_C_COMPILER   NAMES ${COMPILER_PREFIX}gcc)
find_program(CMAKE_ASM_COMPILER NAMES ${COMPILER_PREFIX}gcc)
find_program(CMAKE_CXX_COMPILER NAMES ${COMPILER_PREFIX}g++)

add_compile_options(
    -mcpu=cortex-m3
    -Wall
    -Werror
)

add_link_options(
    -mcpu=cortex-m3
)