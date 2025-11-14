#define STM32C011xx
#include <stm32c0xx.h>


volatile uint32_t ticks;


void SysTick_Handler(void) {
    ticks++;
}


void delay(uint32_t ms) {
    uint32_t start = ticks;
    while(ticks - start < ms);
}


void init_SYSTICK(void) {
    NVIC_SetPriority(SysTick_IRQn, 1);  // set priority 1, the second highest after 0
    NVIC_EnableIRQ(SysTick_IRQn);	    // enable SysTick interrupt in NVIC

    // assume 12 MHz core clock 
    SysTick->LOAD = 12000000/1000 - 1;  // 1 ms counter for 1 kHz interrupt freq.
    SysTick->VAL = 0; 
    SysTick->CTRL = 
        1 << SysTick_CTRL_CLKSOURCE_Pos // use processor clock for SysTick input clock
    |   1 << SysTick_CTRL_TICKINT_Pos   // enable SysTick interrupt generation
    |   1 << SysTick_CTRL_ENABLE_Pos    // enable SysTick counter
    ;
}


void init_LED(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is now on

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


int main(void) {

    init_LED();
    init_SYSTICK();

    for(;;) {
        GPIOB->ODR ^= (1U << 6);    // toggle LED
        delay(500);                 // delay 500 ms
    }
}
