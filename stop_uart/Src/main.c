
// stop mode with active USART1 RX

// AN4991
//  Application note
//  How to wake up an STM32 microcontroller from low-power mode with the USART or the LPUART

#define STM32C011xx
#include <stm32c0xx.h>

#include <ctype.h>

void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE_RXFNE) { 	// RXNE flag set?
        char ch = (char)USART1->RDR; 	        // reading RDR automagically clears the RXNE flag
        USART1->TDR = toupper(ch);              // echo: to uppercase
    } // else: handle more flags here like TXE, overrun error, ...
}


// USART1 on PA9 and PA10 for STM32C011-DK
void init_UART1(void) {
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;   // enable clock for peripheral component 
    (void)RCC->APBENR2; // ensure that the last instruction finished and the clock is now on

    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP; // remap: use  PA9 instead of PA11
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA12_RMP; // remap: use PA10 instead of PA12

    // PA9, PA10 = USART1 TX, RX, routed to ST-LINK VCP, see STM32C011-DK board schematics
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    (void)RCC->IOPENR; // ensure that the last write command finished and the clock is on

    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL9_Msk) | (1 << GPIO_AFRH_AFSEL9_Pos); // AF1 
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE9_Msk) | (2 << GPIO_MODER_MODE9_Pos);   // AF mode

    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL10_Msk) | (1 << GPIO_AFRH_AFSEL10_Pos); // AF1
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE10_Msk) | (2 << GPIO_MODER_MODE10_Pos);   // AF mode

    RCC->APBENR2 |= RCC_APBENR2_USART1EN;
    (void)RCC->APBENR2; // ensure that the last instruction finished and the clock is on
    uint32_t baud_rate = 115200;
    USART1->BRR = (12000000 + (baud_rate/2)) / baud_rate;    // rounded, SYSCLK after reset
    USART1->CR1 = 
        USART_CR1_UE |              // enable USART
        USART_CR1_RE |              // enable receiver
        USART_CR1_RXNEIE_RXFNEIE |  // enable RXNE interrupt
        USART_CR1_UESM |            // enable USART in stop mode
        USART_CR1_TE;               // enable transmitter
    NVIC_EnableIRQ(USART1_IRQn);    // enable interrupt 
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


// MCU runs on 12 MHz SYSCLK clock (HIS48/4), default after reset
int main(void)
{
    RCC->CR = (RCC->CR & ~RCC_CR_HSIDIV_Msk) | (2 << RCC_CR_HSIDIV_Pos);        // HIS48/4 == 12 MHz (default)
    RCC->CR = (RCC->CR & ~RCC_CR_HSIKERDIV_Msk) | (3 << RCC_CR_HSIKERDIV_Pos);  // set HSIKER HIS48/4 = 12 MHz, yes the constant is 3 here, not 2
    //RCC->CR |= RCC_CR_HSIKERON; // enable HSI in run and stop modes - (more power + faster wakeup)
    while(!(RCC->CR & RCC_CR_HSIRDY)); // wait for HSI ready

    RCC->APBSMENR2 |= RCC_APBSMENR2_USART1SMEN; // enable USART1 in sleep and stop modes
    RCC->CCIPR = (RCC_CCIPR_USART1SEL &~RCC_CCIPR_USART1SEL_Msk) | (2 << RCC_CCIPR_USART1SEL_Pos);    // select HSIKER as USART1 clock source
    DBG->CR &= ~DBG_CR_DBG_STOP; 		// disable debug in stop mode
    
    init_UART1();
    /* Loop forever */
    SCB->SCR |= SCB_SCR_SLEEPONEXIT; // optional: re-enter sleep mode after ISR exit
	for(;;) {
        USART1->TDR = '$'; // send a prompt
        while(!(USART1->ISR & USART_ISR_TC));   // wait for UART transmission completed
        enter_stop_mode_wfi();      // never returns when SCB_SCR_SLEEPONEXIT is set
    }
}
