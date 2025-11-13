#define STM32C011xx
#include <stm32c0xx.h>


// set FLASH read latency to 0 or 1 wait states
// this must be done according to the HCLK frequency
// if(HCLK <= 24 MHz) latency=0; else latency=1; 
void set_flash_latency(int latency) {
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | (latency << FLASH_ACR_LATENCY_Pos);
    // must read back register until new value matches, see ref.man.
    while(latency != FLASH_ACR_LATENCY_Msk & (FLASH->ACR >> FLASH_ACR_LATENCY_Pos));
}


// procedures from ref.man:
//
// Increasing HCLK frequency
// 1. Set the LATENCY[2:0] bitfield to correspond to the target HCLK frequency, as per Table 14.
// 2. Read the LATENCY[2:0] bitfield until it returns the value written in the previous step.
// 3. Select the system clock source as required, through the SW[2:0] bitfield of the RCC_CFGR register.
// 4. Set the HCLK clock prescaler as required, through the HPRE[3:0] bitfield of the RCC_CFGR register.
//
// Decreasing HCLK frequency
// 1. Select the system clock source as required, through the SW[2:0] bitfield of the RCC_CFGR register.
// 2. Set the HCLK clock prescaler as required, through the HPRE[3:0] bitfield of the RCC_CFGR register.
// 3. Read the SWS[2:0] bitfield of the RCC_CFGR register until it returns the value set into SW[2:0] in step 1.
// 4. Set the LATENCY[2:0] bitfield to correspond to the target HCLK frequency, as per Table 14.
// The HPRE[3:0] bitfield of the RCC_CFGR register can also be read to check its content.


int main(void)
{
    // After reset, including exit from Stop, or Standby, or Shutdown mode, the default is:
    // HSI48 used (RCC_CR HSION) as clock
    // RCC->CR HSIDIV value is 2 -> dividing by 4: HSISYS == HSI48 / 4
    // RCC->CR SYSDIV value is 0 -> dividing by 1: SYSCLK == HSISYS
    // RCC->CFGR HPRE value is 0 -> dividing by 1: HCLK (AHB) == SYSCLK
    // RCC->CFGR PPRE value is 0 -> dividing by 1: PCLK (APB) == HCLK

    // switch the clock to 48 MHz which is the max. possible clock
    set_flash_latency(1);
    RCC->CFGR = (RCC->CFGR &~RCC_CFGR_HPRE_Msk) | (0 << RCC_CFGR_HPRE_Pos); // AHB prescaler to 0 (1:1)
    RCC->CFGR = (RCC->CFGR &~RCC_CFGR_PPRE_Msk) | (0 << RCC_CFGR_PPRE_Pos); // APB prescaler to 0 (1:1)
    RCC->CFGR = (RCC->CFGR &~RCC_CFGR_SW_Msk) | (0 << RCC_CFGR_SW_Pos);     // System clock switch to HSISYS
    while(0 != (RCC_CFGR_SWS_Msk & RCC->CFGR>>RCC_CFGR_SWS_Pos));           // wait until switch happened

    /* Loop forever */
    for(;;);
}
