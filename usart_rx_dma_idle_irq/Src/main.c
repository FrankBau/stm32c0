#define STM32C011xx
#define LED PB6

#include <stm32c0xx.h>

#include <stdio.h>
#include <string.h>


char rx_buffer[80];	// buffer for incoming chars, max. 80


void uart1_rx_dma() {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;   // this is good for DMA and DMAMUX
    (void)RCC->AHBENR;

    if( DMA1_Channel1->CCR & DMA_CCR_EN) {      // channel was in use before
        DMA1_Channel1->CCR &= ~DMA_CCR_EN;      // disable DMA channel for setup
    }

    // route peripheral DMA request to DMA channel
    // Table 34: DMAMUX usart1_rx_dma == 50
    // caution: DMAMUX1_Channel0 is for DMA1_Channel1 and so on!
    DMAMUX1_Channel0->CCR = 50 << DMAMUX_CxCR_DMAREQ_ID_Pos;
    
    DMA1->IFCR = DMA_IFCR_CGIF1;    // clear all (HT, TC, TE) flags for DMA channel 1

    DMA1_Channel1->CPAR = (uint32_t)&(USART1->RDR);
    DMA1_Channel1->CMAR = (uint32_t)rx_buffer;
    DMA1_Channel1->CNDTR = sizeof(rx_buffer);
    DMA1_Channel1->CCR = 
        0 << DMA_CCR_MEM2MEM_Pos    // MEM2MEM 0: no memory-to-memory mode
    |   0 << DMA_CCR_PL_Pos         // PL priority level 0: low.. 3: very high
    |   0 << DMA_CCR_MSIZE_Pos      // MSIZE 0: 8-bit 1: 16-bit 2: 32-bit
    |   0 << DMA_CCR_PSIZE_Pos      // PSIZE 0: 8-bit 1: 16-bit 2: 32-bit
    |   1 << DMA_CCR_MINC_Pos       // MINC memory increment mode on (1)
    |   0 << DMA_CCR_PINC_Pos       // PINC peripheral increment mode off (0)
    |   0 << DMA_CCR_CIRC_Pos       // CIRC 1: circular mode
    |   0 << DMA_CCR_DIR_Pos        // DIR 0: read from peripheral, 1: memory
    |   0 << DMA_CCR_TEIE_Pos       // TEIE transfer error interrupt 1: enable
    |   0 << DMA_CCR_HTIE_Pos       // HTIE half transfer interrupt 1: enable
    |   0 << DMA_CCR_TCIE_Pos       // TCIE transfer complete interrupt 1: enable
    ;
    DMA1_Channel1->CCR |= DMA_CCR_EN; // enable DMA channel

    // A channel, as soon as enabled, may serve any DMA request from the peripheral 
    // connected to this channel, or may start a memory-to-memory block transfer.
}


void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_IDLE) { // idle line flag set ?
        USART1->ICR |= USART_ICR_IDLECF; // writing 1 *clears* the idle line detected flag
        uint32_t received = sizeof(rx_buffer) - DMA1_Channel1->CNDTR;
        if(received < sizeof(rx_buffer))
            rx_buffer[received] = '\0'; // set terminating '\0' char if enough space

        // evaluation of rx_buffer content
        if (strncmp(rx_buffer, "LED on", 6) == 0) {
            GPIOB->BRR = 1 << 6; // set GPIOB pin 6 low -> LED on (low active)
        } else if (strncmp(rx_buffer, "LED off", 7) == 0) {
            GPIOB->BSRR = 1 << 6; // set GPIOB pin 6 high -> LED off (low active)
        } else {
            // unknown command
        }

        if (USART1->ISR & USART_ISR_ORE) {  // overrun detected
            // this happens when too many chars were received resp. the DMA buffer was too short
            USART1->ICR |= USART_ICR_ORECF; // clear overrun flag
        }
        uart1_rx_dma();                  // init next DMA
    }
}


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


void init_LED(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is now on

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


int main(void) {
    init_LED();
    init_UART1();
    uart1_rx_dma();
    USART1->CR3 |= USART_CR3_DMAR;      // enable reveiver DMA
    USART1->CR1 |= USART_CR1_IDLEIE;    // enable uart idle interrupt
    NVIC_EnableIRQ(USART1_IRQn);        // enable this interrupt in NVIC
    /* Loop forever */
    for(;;);
}
