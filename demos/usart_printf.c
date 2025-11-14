#define STM32C011xx
#define LED PB6

#include <stm32c0xx.h>

#include <stdio.h>


void init_UART1(void) {
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;   // enable clock for peripheral component 
    (void)RCC->APBENR2; // ensure that the last instruction finished and the clock is now on

    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP; // remap PA9 instead of PA11
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA12_RMP; // remap PA10 instead of PA12

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
    USART1->BRR = (12000000 + (baud_rate/2)) / baud_rate;       // rounded, SYSCLK after reset
    USART1->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE;   // enable UART, RX, TX 
}


// implement __io_putchar() to redirect printf() to USART1
// see syscall.c to see how this function is used
int __io_putchar(int ch) {
    while (!(USART1->ISR & USART_ISR_TXE_TXFNF))
        ;   // loop while the TX register is not empty (last transmission not completed)
    USART1->TDR = (uint8_t)ch;  // write the char to the transmit data register
    return ch;                  // indicate success to the caller
}


int main(void) {
    setbuf(stdout, NULL);   		// optional: make stdout unbuffered
    init_UART1();           		// setup USART1
    printf("Hello, world.\n");      // maybe printf("Hello, world.\r\n");
    /* Loop forever */
    for(;;);
}
