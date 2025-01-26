#define STM32C011xx
#define LED PB6

#include <stm32c0xx.h>
#include <gpio.h>


// PA8  is connected to the (analog) joystick of the STM323C0116-DK, 
// but used as an EXTI interrupt pin here

// Note: Pressing a real button often causes bouncing: For a reliable detection of button presses, 
// de-bounding should be implemented (which is in fact easier done by polling than by interrupt).

// make PA8 EXTI interrupt
void init_EXTI() {
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN; // enable peripheral clock
    (void)RCC->IOPENR; // read back to make sure that clock is on
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE8_Msk) | (0 << GPIO_MODER_MODE8_Pos); // PA8 input mode

    // let m be the pin number (0..15)
    // and x be the port number (A=0, B=1,...)
    // for PA8:
    int m = 8;  // pin name m corresponds to reference manual
    int x = 0;  // port name x corresponds to reference manual

    EXTI->FTSR1 |= 1 << m; // enable trigger on falling edge for pin m (0..15)
    
    // there is 1 byte for each port number
    // that is 4 bytes per 32-bit register (layout chosen by CMSIS)
    EXTI->EXTICR[m / 4] &= ~(0xF << (m % 4)); // clear port bits for pin m
    EXTI->EXTICR[m / 4] |=     x << (m % 4);  // set port x for pin m

    EXTI->IMR1 |= EXTI_IMR1_IM8;    // EXTI CPU wakeup with interrupt mask register
    //EXTI->EMR1 |= EXTI_EMR1_EM8;  // EXTI CPU wakeup with event mask register

    NVIC_EnableIRQ(EXTI4_15_IRQn);  // enable EXTI interrupt in NVIC
}


void EXTI4_15_IRQHandler(void) {
    if(EXTI->FPR1 & EXTI_FPR1_FPIF8) {
        // falling edge on PA8 -> IRQ pending
        EXTI->FPR1 |= EXTI_FPR1_FPIF8;  // clear pending bit
        gpio_toggle(LED);               // toggle the LED
    }
}


int main(void) {
    gpio_init();
    gpio_set_mode(LED, 1);  // set LED pin to GPIO output mode
    
    init_EXTI();    // initialize EXTI for PA8

    /* Loop forever */
    for(;;) {
        // do nothing
    }
    return 0;	// unreachable code, main shall never return in embedded software
}
