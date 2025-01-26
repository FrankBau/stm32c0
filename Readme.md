# STM32C0 Register-Level Guide

This is a collection of register-level C code projects 

- using only the [STM32C0 CMSIS headers](https://github.com/STMicroelectronics/STM32CubeC0/tree/main/Drivers/CMSIS) 
- buildable using [Visual Studio Code](https://code.visualstudio.com/)
- with the [STM32 VS Code Extension](https://marketplace.visualstudio.com/items?itemName=stmicroelectronics.stm32-vscode-extension)
- runnable on the [STM32C0116-DK Discovery kit with STM32C011F6 MCU](https://www.st.com/en/evaluation-tools/stm32c0116-dk.html)

But easily portable to other STM32C0 boards like the Nucelo boards and also to the other series of STM32 microcontrollers.

If you prefer command line builds, check out the repo, navigate to a project, and issue from a command prompt:
```
cmake -G Ninja -B build
cmake --build build
```
The final `build/*.bin` file can be simply flashed by dragged it onto the new USB drive that appears when you connect the board via USB to the build host.  

Detailed infos are in the ~100 pages [STM32C0 Register-Level Guide](docs/STM32C0%20Register-Level%20Guide.pdf)

## STM32C0 Projects

- [blinky](https://github.com/FrankBau/stm32c0/tree/main/blinky/) - blink the on-board LED using CMSIS header files only
- [blinky2](https://github.com/FrankBau/stm32c0/tree/main/blinky2/) - blink the on-board LED using CMSIS header files and files from the common folder

