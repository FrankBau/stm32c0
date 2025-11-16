cmake_minimum_required(VERSION 3.20)
project(stm32c0-demos C ASM)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

function(add_demo name)
    add_executable(${name}
        ${CMAKE_SOURCE_DIR}/demos/${name}.c
        ${CMAKE_SOURCE_DIR}/common/Src/syscall.c
        ${CMAKE_SOURCE_DIR}/common/Src/sysmem.c
        ${CMAKE_SOURCE_DIR}/common/Src/startup_stm32c011f6ux.s
    )
    set_target_properties(${name} PROPERTIES SUFFIX ".elf")
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/demos
        ${CMAKE_SOURCE_DIR}/common/Inc
        ${CMAKE_SOURCE_DIR}/common/CMSIS/Core/Include
        ${CMAKE_SOURCE_DIR}/common/CMSIS/Device/ST/STM32C0xx/Include
    )    
    target_link_options(${name} PRIVATE

    )
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
                $<TARGET_FILE:${name}>
                ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin
        COMMENT "Generating ${name}.bin"
    )    
endfunction()

# Add demos
add_demo(adc_multi_dma)
add_demo(adc_single_sw)
add_demo(crc)
add_demo(dma_mem2mem)
add_demo(dma_mem2uart)
add_demo(exti_button)
add_demo(flash_boot_counter)
add_demo(gpio_blinky)
add_demo(gpio_debouncing)
add_demo(i2c_master_rx_tx)
add_demo(mpu)
add_demo(rcc_sysclk)
add_demo(rtc_alarm_irq)
add_demo(spi_master_rx_tx)
add_demo(standby_iwdg)
add_demo(stop_rtc_alarm)
add_demo(stop_uart)
add_demo(systick)
add_demo(tim_pwm_out)
add_demo(tim_upcount)
add_demo(usart_echo)
add_demo(usart_printf)
add_demo(usart_rx_dma_idle_irq)
add_demo(usart_rx_irq)