#define STM32C011xx
#define LED PB6

#include <stm32c0xx.h>
#include <gpio.h>

int main(void)
{
    gpio_init();
    gpio_set_mode(LED, 1);  // set LED pin to GPIO output mode
    
    /* Loop forever */
    for(;;) {
        for (volatile int i = 0; i < 100000; ++i);  // delay
        gpio_set_1(LED);   // set LED pin, output high -> LED off

        for (volatile int i = 0; i < 100000; ++i);  // delay
        gpio_set_0(LED);    // reset LED pin, output low -> LED on
    }
    return 0;	// unreachable code, main shall never return in embedded software
}
