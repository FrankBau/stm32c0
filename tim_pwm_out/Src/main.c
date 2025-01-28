#define STM32C011xx
#include <stm32c0xx.h>


void init_PB6(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is now on

    // set pin PB6 to alternate function TIM3_CH1 (AF12), see data sheet
    GPIOB->AFR[0] = (GPIOB->AFR[0] & ~GPIO_AFRL_AFSEL6_Msk) | (12 << GPIO_AFRL_AFSEL6_Pos); // AF12

    // set pin PB6 to alternate function mode (2), see ref. man.
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (2 << GPIO_MODER_MODE6_Pos);
}


void init_TIM3(void) {
    // assume that all relevant clocks (SYSCLK, HCLK, PCLK, TPCLK) are at 12 MHz (default)
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;  // enable TIM3 clock
    (void)RCC->APBENR1; // ensure that the last write command finished and the clock is on

    TIM3->PSC =   12-1; 	// set pre-scaler such that CNT will increment every microsecond
    TIM3->ARR = 1000-1;		// set counter period to 1 millisecond 

    // set timer TIM3 channel CH1 to PWM output 0b00
    TIM3->CCMR1 = (TIM3->CCMR1 &~TIM_CCMR1_CC1S_Msk) | (0 << TIM_CCMR1_CC1S_Pos);

    // 0b0110 CH1  PWM mode 1: In up-counting, channel 1 is active as long as CNT < CCR1
    TIM3->CCMR1 = (TIM3->CCMR1 &~TIM_CCMR1_OC1M_Msk) | (6<<TIM_CCMR1_OC1M_Pos);
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE; 	// OC preload enable (see reference manual)
    TIM3->CCER |= TIM_CCER_CC1E;     	// enable CH1 and channel output pin
    TIM3->CCR1 = 0;  			        // set length of active (high) phase of PWM pulse

    TIM3->CR1 |= TIM_CR1_CEN;  	// enable the timer, this starts counting
}


// outlook: 
// - enable TIM3 update interrupt, 
// - count milliseconds in the interrupt handler,
// - implement (another) proper delay(ms) function


int main(void) {

    init_PB6();
    init_TIM3();

    /* Loop forever */
    for(;;) {
        for(volatile int i=0; i<1000; ++i);     // some delay
        TIM3->CCR1 = (TIM3->CCR1 + 1) % 1000;   // slowly increase PWM duty period
    }
}

