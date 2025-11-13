cmake_minimum_required(VERSION 3.20)
project(stm32c0-demos C ASM)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(add_demo name)
    add_executable(${name}
        ${CMAKE_SOURCE_DIR}/demos/${name}/main.c
        ${CMAKE_SOURCE_DIR}/common/Src/startup_stm32c011f6ux.s
        ${CMAKE_SOURCE_DIR}/common/Src/syscalls.c
        ${CMAKE_SOURCE_DIR}/common/Src/sysmem.c
    )
    set_target_properties(${name} PROPERTIES SUFFIX ".elf")
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/demos/${name}
        ${CMAKE_SOURCE_DIR}/common/Inc
        ${CMAKE_SOURCE_DIR}/common/CMSIS/Core/Include
        ${CMAKE_SOURCE_DIR}/common/CMSIS/Device/ST/STM32C0xx/Include
    )    
    target_link_options(${name} PRIVATE
        -T${CMAKE_SOURCE_DIR}/linker/stm32c011f6ux_FLASH.ld
    )
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
                $<TARGET_FILE:${name}>
                ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin
        COMMENT "Generating ${name}.bin"
    )    
endfunction()

# Add demos
add_demo(gpio_blinky)
