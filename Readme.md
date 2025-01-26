# STM32C0 Register-Level Guide


## The Guide

The [STM32C0 Register-Level Guide](docs/STM32C0%20Register-Level%20Guide.pdf) is an attempt to get you started with STM32 microcontroller development by following a single, about 100 pages document.
It uses a simple VS Code workflow and points you to the relevant documentation and additional material.


## The Examples

The examples are discussed in the guide. They

- are register-level C-code projects 
- use only the [STM32C0 CMSIS headers](https://github.com/STMicroelectronics/STM32CubeC0/tree/main/Drivers/CMSIS) 
- build using [Visual Studio Code](https://code.visualstudio.com/)
- with the [STM32 VS Code Extension](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension)
- run on the [STM32C0116-DK Discovery kit with STM32C011F6 MCU](https://www.st.com/en/evaluation-tools/stm32c0116-dk.html)

They should be easily portable to other STM32C0 boards like the Nucleo boards and also to the other series of STM32 microcontrollers.

If you prefer command line builds, check out the [repo](https://github.com/FrankBau/stm32c0/), navigate to a project, and issue from a command prompt:
```
cmake -G Ninja -B build
cmake --build build
```
The final `build/*.bin` binary image file can be simply flashed by dragged it onto the new USB drive that appears when you connect the board via USB to the build host. But, of cause, interactive debugging gives you deeper insights.

## Example List

### GPIO

- [blinky](https://github.com/FrankBau/stm32c0/tree/main/blinky/) - blink the on-board LED using CMSIS header files only
- [blinky2](https://github.com/FrankBau/stm32c0/tree/main/blinky2/) - blink the on-board LED using CMSIS header files and files from the common folder
- [exti_button](https://github.com/FrankBau/stm32c0/tree/main/exti_button)

### UART

- [usart_echo](https://github.com/FrankBau/stm32c0/tree/main/usart_echo)
- [usart_printf](https://github.com/FrankBau/stm32c0/tree/main/usart_printf)
- [usart_rx_dma_idle_irq](https://github.com/FrankBau/stm32c0/tree/main/usart_rx_dma_idle_irq)
- [usart_rx_irq](https://github.com/FrankBau/stm32c0/tree/main/usart_rx_irq)

### DMA

- [dma_mem2mem](https://github.com/FrankBau/stm32c0/tree/main/dma_mem2mem)
- [dma_mem2uart](https://github.com/FrankBau/stm32c0/tree/main/dma_mem2uart)

### SPI

### I2C

### Timer

### ADC

### IWDG

### RTC

### Flash

### RCC

### PWR

### Arm Cortex-M0+
