
// stop mode with RTC alarm

#define STM32C011xx
#include <stm32c0xx.h>

#include <ctype.h>


void rtc_init(void) {
    // RTC peripheral clock init
    RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN; // enable clock for peripheral
    (void)RCC->APBENR1;   // ensure that the last write finished and the clock is now on

    // RTCCLK clock init, the clock for date&time, this is not the clock for the peripheral
    // The functionality in Stop mode (including wake-up) is supported only when the clock is LSI or LSE.
    RCC->CSR2 |= RCC_CSR2_LSION;    // low speed internal oscillator (LSI) on
    while((RCC->CSR2 & RCC_CSR2_LSIRDY) != RCC_CSR2_LSIRDY); // wait for LSI ready    
    // set LSI as RTCCLK clock source
    RCC->CSR1 = (RCC->CSR1 &~RCC_CSR1_RTCSEL_Msk) | (2<<RCC_CSR1_RTCSEL_Pos);
    RCC->CSR1 |= RCC_CSR1_RTCEN;    // enable RTCCLK (clock for timing)
}


void rtc_set_alarm_it(void) {
    RTC->WPR = 0xCA;    // disable RTC register write protection, step 1
    RTC->WPR = 0x53;    // disable RTC register write protection, step 2

    RTC->CR &=~RTC_CR_ALRAIE;   // Alarm A interrupt disabled
    RTC->CR &=~RTC_CR_ALRAE;    // Alarm A disabled

    RTC->SCR |= RTC_SCR_CALRAF; // Writing a 1 clears the ALRAF bit in the RTC_SR register.

    while(!(RTC->ICSR & RTC_ICSR_ALRAWF)); // wait until alarm registers can be changed
    // set RTC alarm A every minute, when seconds are 42 
    RTC->ALRMAR =
            1 << RTC_ALRMAR_MSK4_Pos    // Date/day don’t care in alarm A comparison
        |   1 << RTC_ALRMAR_MSK3_Pos    // Hours don’t care in alarm A comparison
        |   1 << RTC_ALRMAR_MSK2_Pos    // Minutes don’t care in alarm A comparison
        |   0 << RTC_ALRMAR_MSK1_Pos    // Seconds *do* care in alarm A comparison
        |   4 << RTC_ALRMAR_ST_Pos      // Second tens in BCD format.
        |   2 << RTC_ALRMAR_SU_Pos      // Second units in BCD format.
        ;
    RTC->ALRMASSR = 
        0 << RTC_ALRMASSR_MASKSS_Pos    // no subsecond bits are used for matching
    |   0 << RTC_ALRMASSR_SS_Pos;       // subseconds (SS) match value set to 0

    RTC->CR |= RTC_CR_ALRAE;    // Alarm A enabled
    RTC->CR |= RTC_CR_ALRAIE;   // Alarm A interrupt enabled

    EXTI->IMR1 |= EXTI_IMR1_IM19;   // enable RTC interrupt on RTC EXTI line (==19)
    NVIC_EnableIRQ(RTC_IRQn);	    // enable RTC interrupt on NVIC

    // enable RTC register write protection
    RTC->WPR = 0xFF;    
}

void blink(void) {
    for (volatile int i = 0; i < 100000; ++i);  // delay
    GPIOB->BRR = 1 << 6;    // reset pin PB6 output low -> LED on

    for (volatile int i = 0; i < 100000; ++i);  // delay
    GPIOB->BSRR = 1 << 6;   // set pin PB6 output high -> LED off
}


void RTC_IRQHandler(void) {
    if(RTC->CR &  RTC_CR_ALRAIE) {
        if(RTC->SR & RTC_SR_ALRAF) {
            RTC->SCR = RTC_SCR_CALRAF; // writing 1 clears the ALRAF bit in RTC_SR
            // add your code
        }
    }
}


void enter_stop_mode_wfi() {
    RCC->APBENR1 |= RCC_APBENR1_PWREN;
    (void)RCC->APBENR1;
    PWR->CR1 = 0;   //  LPMS[2:0]
    SCB->SCR |= (1 << SCB_SCR_SLEEPDEEP_Pos); // enable MCU deep sleep
    __DSB();
    __WFI();                                  // wait for interrupt, zzz…
    SCB->SCR &=~(1 << SCB_SCR_SLEEPDEEP_Pos); // disable MCU deep sleep again
}


// LED pin PB6 setup
// note that the LED low active: setting PB6 to high will turn off the LED
void init_LED(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR;                  // make sure that the clock is on by now

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


// MCU runs on 12 MHz SYSCLK clock (HIS48/4), default after reset
int main(void)
{
    RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (2 << RCC_CR_HSIDIV_Pos);        // HIS48/4 == 12 MHz (default)
    RCC->CR = (RCC->CR & ~RCC_CR_HSIKERDIV_Msk) | ((4-1) << RCC_CR_HSIKERDIV_Pos);  // set HSIKER HIS48/4 = 12 MHz
    //RCC->CR |= RCC_CR_HSIKERON;       // enable HSI in run and stop modes - (more power + faster wakeup)
    while(!(RCC->CR & RCC_CR_HSIRDY));  // wait for HSI ready

    DBG->CR &= ~DBG_CR_DBG_STOP; 		// disable debug in stop mode

    init_LED();
    rtc_init();
    rtc_set_alarm_it();

    /* Loop forever */
	for(;;) {
        enter_stop_mode_wfi();      // stop until RTC alarm
        GPIOB->ODR ^= 1 << 6;       // toggle LED as a sign of life
    }
}
