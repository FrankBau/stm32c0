#define STM32C011xx     // define the target microcontroller
#include <stm32c0xx.h>  // include the CMSIS main header file

#include "gpio.h"

int main(void)          // main function, the entry point of the gpio_blinky program
{
    gpio_setup(PB6,  (gpio_setup_t){.mode = OUTPUT});

    // loop forever
    for(;;) {
        for (volatile int i = 0; i < 100000; ++i);  // some short delay
        gpio_set_0(PB6);
        for (volatile int i = 0; i < 100000; ++i);  // some short delay
        gpio_set_1(PB6);
    }
    return 0;  // unreachable code
}
