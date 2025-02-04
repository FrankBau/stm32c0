#define STM32C011xx

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
    USART1->BRR = 12000000 / 115200; // for SYSCLK = 12MHz and baud rate 115200
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


void mem2mem_dma() {
    static char src[64] = "The quick brown fox jumped over the lazy dog.";  // static!
    char dst[64] = {0}; // gcc will clear it with memset or equivalent

    RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable peripheral clock
    (void)RCC->AHBENR; // read back to make sure that clock is on

    DMA1_Channel1->CCR &= ~DMA_CCR_EN;   // disable DMA channel for setup
    DMA1->IFCR = DMA_IFCR_CGIF1;    	// clear all (HT, TC, TE) flags for DMA channel 1
    
    DMA1_Channel1->CPAR = (uint32_t)src; // source address for the transfer
    DMA1_Channel1->CMAR = (uint32_t)dst; // destination address for the transfer
    DMA1_Channel1->CNDTR = sizeof(src);	 // number of data items to be transferred
    DMA1_Channel1->CCR = 
        1 << DMA_CCR_MEM2MEM_Pos    // MEM2MEM 1: memory-to-memory mode
    |   0 << DMA_CCR_PL_Pos         // PL priority level 0: low.. 3: very high
    |   0 << DMA_CCR_MSIZE_Pos      // MSIZE 0: 8-bit 1: 16-bit 2: 32-bit
    |   0 << DMA_CCR_PSIZE_Pos      // PSIZE 0: 8-bit 1: 16-bit 2: 32-bit
    |   1 << DMA_CCR_MINC_Pos       // MINC memory increment mode	1: enable
    |   1 << DMA_CCR_PINC_Pos       // PINC peripheral increment mode	1: enable
    |   0 << DMA_CCR_CIRC_Pos       // CIRC 0 : normal (linear) DMA	1: circular DMA
    |   0 << DMA_CCR_DIR_Pos        // DIR 0: read from peripheral, 	1: memory
    |   0 << DMA_CCR_TEIE_Pos       // TEIE transfer error interrupt 	1: enable
    |   0 << DMA_CCR_HTIE_Pos       // HTIE half transfer interrupt 	1: enable
    |   0 << DMA_CCR_TCIE_Pos       // TCIE transfer complete interrupt 1: enable
    |   1 << DMA_CCR_EN_Pos         // EN : set 1 to enable DMA channel 
    ;

    // this works only because UART transmission by CPU core is slower than DMA 
    puts(dst);  // output string. Caution: DMA transfer and puts are running concurrently !!!  
}


int main(void) {
    init_UART1();           		// setup USART1
    mem2mem_dma();          		// setup DMA1 channel 1
    /* Loop forever */
    for(;;);
}
