#define STM32C011xx
#include <stm32c0xx.h>


void init_LED(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is now on

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


void init_TIM14(void) {
  // assume that all relevant clocks (SYSCLK, HCLK, PCLK, TPCLK) are at 12 MHz (default)
  RCC->APBENR2 |= RCC_APBENR2_TIM14EN;  // enable TIM14 clock
  (void)RCC->APBENR2; // ensure that the last write command finished and the clock is on
  
  // config TIM14 in up-counter mode. With 12 MHz clock input (CK_INT),
  // the timer will, after pre-scaling, count milliseconds 0, 1, 2,...

  // PSC[15:0] Pre-scaler register - divides the counter clock by factor PSC+1. (reset value: 0x0000)
  TIM14->PSC = 12000-1; 		// set PSC such that CNT will increment every millisecond

  // ARR[15:0] Auto-reload register - Counter counts 0..ARR-1 (reset value: 0xFFFF)
  //TIM14->ARR = 1000-1;		// limit counter period (if needed)

  // CNT[15:0] Counter register - current value of the counter (reset value: 0x0000)
  //TIM14->CNT = 0;       	// no need to set / change the default
  
  TIM14->CR1 |= TIM_CR1_CEN;  	// enable the timer, this starts counting
}


// wait for ms milliseconds
// ms must be in range 1 .. 2^16-1 
void delay(uint16_t ms) {
    uint16_t start = TIM14->CNT;        // current count
    // subtraction of two uint16_t is always correct mod 2^16
    // even in the case of underflow (due to wrap around)
    while((uint16_t)(TIM14->CNT-start) < ms);   // wait until done
}


int main(void) {

    init_LED();
    init_TIM14();
    // next line is an debugging aid. 
    // it prevents the peripheral(s) from running while the debugger halted the CPU 
    DBG->APBFZ2 |= DBG_APB_FZ2_DBG_TIM14_STOP; // pause TIM14 while debugger halts CPU

    /* Loop forever, blink at 1 Hz */
    for(;;) {
        GPIOB->BRR = 1 << 6;    // set GPIOB pin 6 low -> LED (low active) on
        delay(100);        
        GPIOB->BSRR = 1 << 6;   // set GPIOB pin 6 high -> LED (low active) off
        delay(900);
    }
}
