#include <stm32c011xx.h>
#include <gpio.h>

#define LED PB6

int main(void)
{
    gpio_init();
    gpio_output(LED);
    
    /* Loop forever */
    for(;;) {
        for (volatile int i = 0; i < 100000; ++i);  // delay
        gpio_set_1(LED);   // set LED pin, output high -> LED off

        for (volatile int i = 0; i < 100000; ++i);  // delay
        gpio_set_0(LED);    // reset LED pin, output low -> LED on
    }
    return 0;	// unreachable code, main shall never return in embedded software
}
