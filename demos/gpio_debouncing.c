#define STM32C011xx
#include <stm32c0xx.h>


// demonstrating debouncing of a button
// - a short button press will turn the LED on
// - a long button press will turn the LED off 
// the button press is actually evaluated when the button goes up again


// LED pin PB6 setup
// note that the LED low active: setting PB6 to high will turn off the LED
void init_LED(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR;                  // make sure that the clock is on by now

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


// init the joystick button at pin PA8
// when this button is pressed down, PA8 is connected to GND
// we activate the internal pull-up resistor of that GPIO pin
// therefore, the button would also work without any external pull-up resistors
void init_Button(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;  // enable clock for peripheral component GPIOA
    (void)RCC->IOPENR;                  // ensure that the last write command finished and the clock is on
    // set pin mode to input (0). 
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE8_Msk) | (0 << GPIO_MODER_MODE8_Pos);
    // enable internal pull-up resistor (1).
    GPIOA->PUPDR = (GPIOA->PUPDR & ~GPIO_PUPDR_PUPD8_Msk) | (1 << GPIO_PUPDR_PUPD8_Pos);
    // Reset defaults for other GPIO registers are okay here
}


int main(void)
{
    init_LED(); // initialize the LED pin
    init_Button(); // initialize the button pin
    
    int down_count = 0;
    // loop forever
    for(;;) {
        if(!(GPIOA->IDR & GPIO_IDR_ID8)) {
            // button down detected
            down_count++;
        } else {
            // button up detected
            if(down_count > 50) {
                // long button press detected
                GPIOB->BSRR = 1 << 6; 	// set GPIOB pin 6 high -> LED (low active) off	
            } else if(down_count > 5) {
                // short button press detected
                  GPIOB->BRR = 1 << 6; 	// set GPIOB pin 6 low -> LED (low active) on
            }
            down_count = 0;
        }
        for(volatile int i=0; i<10000; ++i); // some delay. better use precise delay function here
    }

    return 0;	// unreachable code, main shall never return in embedded software
}
