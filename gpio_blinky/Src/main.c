#define STM32C011xx
#include <stm32c0xx.h>


int main(void)
{
    // LED pin setup
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is on by now

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);

    // loop forever
    for(;;) {
        for (volatile int i = 0; i < 100000; ++i);  // delay
        GPIOB->BSRR = 1 << 6;   // set pin PB6 output high -> LED off

        for (volatile int i = 0; i < 100000; ++i);  // delay
        GPIOB->BRR = 1 << 6;    // reset pin PB6 output low -> LED on
    }
    return 0;	// unreachable code, main shall never return in embedded software
}
