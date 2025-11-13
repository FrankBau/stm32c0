set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(CMAKE_C_COMPILER arm-none-eabi-gcc)
find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb -O2" CACHE STRING "" FORCE)
