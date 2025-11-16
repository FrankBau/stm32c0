#define STM32C011xx     // define the target microcontroller
#include <stm32c0xx.h>  // include the CMSIS main header file

#include "gpio.h"

void init_SPI(void)
{   // 25.5.7 Configuration of SPI (RM490)
    // 1. Write proper GPIO registers

    gpio_setup(PA7, (gpio_setup_t){.mode = ALTERNATE, .af = AF0});    // PA7 -> MOSI
    gpio_setup(PA6, (gpio_setup_t){.mode = ALTERNATE, .af = AF0});    // PA6 -> MISO
    gpio_setup(PA5, (gpio_setup_t){.mode = ALTERNATE, .af = AF0});    // PA5 -> SCK
    gpio_setup(PA4, (gpio_setup_t){.mode = ALTERNATE, .af = AF0});    // PA4 -> NSS

    RCC->APBENR2 |= RCC_APBENR2_SPI1EN; // enable clock for this peripheral
    (void)RCC->APBENR2;                 // ensure that enable instruction completed and the clock is on

    // 2.  Write to the SPI_CR1 register:
    SPI1->CR1 =
        3 << SPI_CR1_BR_Pos         //  BR[2:0] baud rate. 3 (0b011): f_PCLK/16
        | 0 << SPI_CR1_CPHA_Pos     // clock phase. data bit sampled at: 0=first clock edge; 1=second
        | 0 << SPI_CR1_CPOL_Pos     // clock polarity: 0: clock low when idle; 1=high when idle
        | 0 << SPI_CR1_LSBFIRST_Pos // least significant bit (LSB): 0=LSB last, 1=LSB first
        | 0 << SPI_CR1_SSM_Pos      // software slave mgmt: 0: disabled (NSS pin used) 1: enabled (SSI bit used)
        | 1 << SPI_CR1_SSI_Pos      // internal slave select bit (when NSS pin is not used, SSM=0)
        | 1 << SPI_CR1_MSTR_Pos     // 0=slave configuration; 1=master configuration
        ;

    // 3.  Write to the SPI_CR2 register:
    SPI1->CR2 =
        (8 - 1) << SPI_CR2_DS_Pos   // DS: data size (x+1 bit). Here: 8 bit (1 byte)
        | 1 << SPI_CR2_SSOE_Pos     // 1: NSS pin is output, managed by the hardware, for single-master
        | 0 << SPI_CR2_FRF_Pos      // FRF frame format: Motorola (default) mode, not TI (special) mode
        | 1 << SPI_CR2_NSSP_Pos     // 1: generate NSS pulse after each byte. (works only when CPHA==0)
        | 1 << SPI_CR2_FRXTH_Pos    // FRXTH (RX FIFO threshold) must be 1 for 8-bit transfers
        ;
    
    // 4. Write to SPI_CRCPR register: Configure the CRC polynomial if needed. (not used)
    
    // 5. Write proper DMA registers (not used)
    
    SPI1->CR1 |= SPI_CR1_SPE; // SPI enable
}

int main(void)
{
    init_SPI();

    uint8_t tx[4] = {0x5A, 0x00, 0xFF, 0xA5};
    uint8_t rx[4] = {0};

    for (unsigned i = 0; i < sizeof(tx) / sizeof(tx[0]); ++i)
    {
        while (!(SPI1->SR & SPI_SR_TXE))
            ; // wait for TXE (transmit buffer empty)

        // put next byte to send into data register
        // type cast needed to prevent integer promotion
        *(__IO uint8_t *)(&SPI1->DR) = (uint8_t)tx[i];

        // hardware writes value of DR to MOSI and simultaneously reads new DR value from MISO

        while (!(SPI1->SR & SPI_SR_RXNE))
            ; // wait for RXNE (receive buffer not empty)

        // get next byte received from data register
        // type cast needed to prevent integer promotion
        rx[i] = *(__IO uint8_t *)(&SPI1->DR);
        (void)rx[i];  // avoid 'set but not used' warning
    }

    /* Loop forever */
    for (;;)
        ;
}
