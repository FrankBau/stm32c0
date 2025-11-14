#define STM32C011xx
#include <stm32c0xx.h>


#include <stdio.h>


// AN4187  Application note "Using the CRC peripheral on STM32 microcontrollers"

// size in bytes, must be a multiple of 4
uint32_t crc32_hw(const uint32_t *data, size_t size)
{

  RCC->AHBENR |= RCC_AHBENR_CRCEN;      // enable peripheral clock
  (void)RCC->AHBENR;                    // read back to ensure that the clock is now running

  // settings for standard CRC-32 polynomial (ISO-HDLC)
  // Poly	    Init	    RefIn	RefOut	XorOut
  // 04C11DB7	FFFFFFFF	True	True	FFFFFFFF
  // see also https://m0agx.eu/2021/04/09/matching-stm32-hardware-crc-with-standard-crc-32/

  CRC->INIT = 0xFFFFFFFF; // initial crc value (reset value)
  CRC->POL = 0x04C11DB7;  // CRC-32 polynomial (reset value) – normal representation
  CRC->CR = CRC_CR_RESET; // reset CRC peripheral
  CRC->CR =
      1 << CRC_CR_REV_OUT_Pos    // 1: Bit-reversed output format
      | 3 << CRC_CR_REV_IN_Pos   // 11: Bit reversal done by word
      | 0 << CRC_CR_POLYSIZE_Pos // 00: 32 bit polynomial
      ;

  for (size_t i = 0; i < size; i+=sizeof(uint32_t))
  {
    CRC->DR = *data;
    data++;
  }
  return ~CRC->DR; // final bit reversal
}


#include <stdint.h>
#include <stdio.h>

uint32_t crc32_sw(const uint8_t *buf, size_t len) {
    uint32_t crc = 0;
    crc = ~crc;
    while (len--) {
        crc ^= *buf++;
        for (int i = 0; i < 8; i++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return ~crc;
}


int main(void)
{
    uint32_t crc_hw = crc32_hw((uint32_t*)"The quick brown fox jumps over the lazy dog.", 44);
    uint32_t crc_sw = crc32_sw( (uint8_t*)"The quick brown fox jumps over the lazy dog.", 44);
    
    if(crc_hw == crc_sw)    // == 0x519025e9
        printf("okay\n");   // redirect stdout to UART or set a breakpoint here
    else
        printf("error\n");  // should never happen

    // loop forever
    for(;;) 
        ;

    return 0;	// unreachable code, main shall never return in embedded software
}
