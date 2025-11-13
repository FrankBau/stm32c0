#define STM32C011xx     // define the target microcontroller
#include <stm32c0xx.h>  // include the CMSIS main header file

int main(void)          // main function, the entry point of the gpio_blinky program
{
    // LED pin setup
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register, see the Reference Manual (RM0490) 6.2.15

    // set the pin PB6 to GPIO **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);

    // loop forever
    for(;;) {
        for (volatile int i = 0; i < 100000; ++i);  // some short delay
        // set bit 6 in BSRR register of GPIOB to 1 -> output level of pin PB6 goes high -> LED off
        GPIOB->BSRR = GPIO_BSRR_BS6;

        for (volatile int i = 0; i < 100000; ++i);  // some short delay
        // set bit 6 in BRR register of GPIOB to 1 -> output level of pin PB6 goes low -> LED on
        GPIOB->BRR = GPIO_BRR_BR6;
    }
    return 0;  // unreachable code
}
