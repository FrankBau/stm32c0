# STM32C0 Register-Level Guide

## The Guide

... is a single, about 100 pages document which gets you started with STM32 microcontroller software development:

[STM32C0 Register-Level Guide](docs/STM32C0%20Register-Level%20Guide.pdf).

## The Examples

Many examples are discussed in the guide. They

- are register-level C-code projects 
- use only the [STM32C0 CMSIS headers](https://github.com/STMicroelectronics/STM32CubeC0/tree/main/Drivers/CMSIS) 
- build using [Visual Studio Code](https://code.visualstudio.com/)
- with the [STM32 VS Code Extension](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension)
- run on the [STM32C0116-DK Discovery kit with STM32C011F6 MCU](https://www.st.com/en/evaluation-tools/stm32c0116-dk.html)

for the details, consult the guide.

The examples should be easily portable to other STM32C0 boards like the Nucleo boards and also to the other series of STM32 microcontrollers.

## Example List

### GPIO

- [gpio_blinky](https://github.com/FrankBau/stm32c0/tree/main/gpio_blinky/) - blink the on-board LED using CMSIS header files only
- [gpio_debouncing](https://github.com/FrankBau/stm32c0/tree/main/gpio_debouncing/) - debounce a button by polling
- [exti_button](https://github.com/FrankBau/stm32c0/tree/main/exti_button) - trigger EXTI interrupt from button press

### UART

- [usart_echo](https://github.com/FrankBau/stm32c0/tree/main/usart_echo)
- [usart_printf](https://github.com/FrankBau/stm32c0/tree/main/usart_printf) - redirect stdout (printf) to debug host
- [usart_rx_dma_idle_irq](https://github.com/FrankBau/stm32c0/tree/main/usart_rx_dma_idle_irq)
- [usart_rx_irq](https://github.com/FrankBau/stm32c0/tree/main/usart_rx_irq)

### DMA

- [dma_mem2mem](https://github.com/FrankBau/stm32c0/tree/main/dma_mem2mem)
- [dma_mem2uart](https://github.com/FrankBau/stm32c0/tree/main/dma_mem2uart) - 

### SPI

### I2C

### Timer

- [tim_upcount](https://github.com/FrankBau/stm32c0/tree/main/tim_upcount) - set timer TIM14 in upcounting mode. used for a delay function
- [tim_pwm_out](https://github.com/FrankBau/stm32c0/tree/main/tim_pwm_out) - LED dimming using a timer channel in PWM output mode

### ADC

 - [adc_single_sw](https://github.com/FrankBau/stm32c0/tree/main/adc_single_sw) - single channel, software triggered ADC analog voltage measurement
 - [adc_multi_dma](https://github.com/FrankBau/stm32c0/tree/main/adc_multi_dma) - multi channel ADC measurements, including MCU temperature and Vdda

### IWDG

### RTC

- [rtc_alarm_irq](https://github.com/FrankBau/stm32c0/tree/main/rtc_alarm_irq) - set a periodic RTC alarm interrupt, used to toggle a LED once per minute

### Flash

- [flash_boot_counter](https://github.com/FrankBau/stm32c0/tree/main/flash_boot_counter) - storing and incrementing a persistent boot counter at the end of the flash memory

### RCC

- [rcc_sysclk](https://github.com/FrankBau/stm32c0/tree/main/rcc_sysclk) - set the SYSCLK to 24 MHz


### PWR

- [stop_uart](https://github.com/FrankBau/stm32c0/tree/main/stop_uart) - entering the stop low-power mode with wakeup from USART1 receive 
- [standby_iwdg](https://github.com/FrankBau/stm32c0/tree/main/standby_iwdg) - entering the standby low-power mode with periodic wakeup by IWDG 


### CRC

- [crc](https://github.com/FrankBau/stm32c0/tree/main/crc) - show crc-32 calculation for a message in software and hardware and compare the results


### Arm Cortex-M0+

- [systick](https://github.com/FrankBau/stm32c0/tree/main/systick) - SysTick delay
- [mpu](https://github.com/FrankBau/stm32c0/tree/main/mpu) - Memory Protection Unit - protect against NULL pointer dereferencing 
