#define STM32C011xx
#include <stm32c0xx.h>

#include <stdio.h>


void process_line(const char text[]) {
    // ... do something with the received text here
    // remember that you are in an irq context, so keep it short and simple
}


void USART1_IRQHandler(void) {
    static char rx_buffer[64];
    static size_t rx_index;
    if (USART1->ISR & USART_ISR_RXNE_RXFNE) { 	// RXNE flag set?
        char c = USART1->RDR; // read character, this clears the interrupt flag
        if(c=='\n') {
            rx_buffer[rx_index] = '\0'; // terminate string
            process_line(rx_buffer);
            rx_index = 0;
        }
        else if(rx_index < sizeof(rx_buffer)-1) {
            rx_buffer[rx_index++] = c;
        }
    } // else: handle more flags here like TXE, overrun error, ...
}


int main(void) {
    // setup PA9, PA10 for USART1 TX, RX (routed to ST-LINK VCP, see STM32C011-DK board schematics)
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
    (void)RCC->IOPENR; // ensure that the last write command finished and the clock is on

    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL9_Msk) | (1 << GPIO_AFRH_AFSEL9_Pos); // AF1 
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE9_Msk) | (2 << GPIO_MODER_MODE9_Pos);   // AF mode

    GPIOA->AFR[1] = (GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL10_Msk) | (1 << GPIO_AFRH_AFSEL10_Pos); // AF1
    GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE10_Msk) | (2 << GPIO_MODER_MODE10_Pos);   // AF mode

    // remap PA9 and PA10
    RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;   // enable clock for peripheral component 
    (void)RCC->APBENR2; // ensure that the last instruction finished and the clock is on

    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP; // remap PA9 instead of PA11
    SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA12_RMP; // remap PA10 instead of PA12

    // setup UART
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;
    (void)RCC->APBENR2; // ensure that the last instruction finished and the clock is on
    uint32_t baud_rate = 115200;
    USART1->BRR = (12000000 + (baud_rate/2)) / baud_rate;    // rounded, SYSCLK after reset
    // enable UART including RXNE interrupt generation in USART peripheral
    USART1->CR1 = USART_CR1_UE | USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE_RXFNEIE;

    NVIC_EnableIRQ(USART1_IRQn);        // enable interrupt handling in NVIC
    USART1->TDR = '>'; // == ASCII code 62 == 0x2E; greet the other side with a prompt
    /* Loop forever */
    for(;;);
}
