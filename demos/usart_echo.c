#define STM32C011xx
#include <stm32c0xx.h>

#include <stdio.h>


// USART1 on PA9 and PA10 for STM32C011-DK
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


char rot13(char ch)
{
    if ('0' <= ch && ch <= '9')
        ch = '0' + (ch - '0' + 5) % 10;
    else if ('A' <= ch && ch <= 'Z')
        ch = 'A' + (ch - 'A' + 13) % 26;
    else if ('a' <= ch && ch <= 'z')
        ch = 'a' + (ch - 'a' + 13) % 26;
    return ch;
}


int main(void)
{
    init_UART1();
    USART1->TDR = '>'; // == ASCII code 62 == 0x2E; greet the other side with a prompt
    while (1) {
        while (!(USART1->ISR & USART_ISR_RXNE_RXFNE))
            ;	// loop while the RX register is empty (nothing received)
        char ch = (char)USART1->RDR;	// read the byte received
        ch = rot13(ch);			// apply the rot13 scrambling algorithm
        while (!(USART1->ISR & USART_ISR_TXE_TXFNF))
            ;	// loop while the TX register is not empty (last transmission not completed)
        USART1->TDR = (uint8_t)ch;	// write the char to transmit
    }
    return 0;
}
