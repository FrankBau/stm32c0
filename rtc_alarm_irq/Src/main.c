#define STM32C011xx
#include <stm32c0xx.h>


// set a periodic RTC alarm interrupt
// used to toggle the user LED  to once per minute.


void init_LED(void) {
        // LED pin setup
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is on by now

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


void rtc_init(void) {
    // RTC peripheral clock init
    RCC->APBENR1 |= RCC_APBENR1_RTCAPBEN; // enable clock for peripheral
    (void)RCC->APBENR1;   // ensure that the last write finished and the clock is now on

    // RTCCLK clock init, the clock for date&time, this is not the clock for the peripheral
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


void RTC_IRQHandler(void) {
    if(RTC->CR &  RTC_CR_ALRAIE) {
        if(RTC->SR & RTC_SR_ALRAF) {
            RTC->SCR = RTC_SCR_CALRAF; // writing 1 clears the ALRAF bit in RTC_SR
            // do something on RTC alarm: toggle LED (LED pin must be initialized before)
            GPIOB->ODR ^= GPIO_ODR_OD6;
        }
    }
}


int main(void)
{
    init_LED();

    rtc_init();
    //rtc_set_time_date();
    rtc_set_alarm_it();

    /* Loop forever */
    for(;;) {
        // enter_stop_mode_wfi();
    }
}
