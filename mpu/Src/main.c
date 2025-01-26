#define STM32C011xx
#include <stm32c0xx.h>


#include <stdlib.h>


// - https://interrupt.memfault.com/blog/fix-bugs-and-secure-firmware-with-the-mpu
// - AN4838 Application note "Introduction to memory protection unit management on STM32 MCUs"
// - https://interrupt.memfault.com/blog/cortex-m-hardfault-debug
// - https://mcuoneclipse.com/2012/11/24/debugging-hard-faults-on-arm-cortex-m/


void HardFault_Handler(void)
{
    GPIOB->BRR = GPIO_BRR_BR6;  // set PB6 output low, turn on the LED
    for(;;);
}


void init_LED(void) {
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR; // read back the register to make sure that the clock is now on

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);

    GPIOB->BSRR = GPIO_BSRR_BS6;  // set PB6 output high, turn off the LED
}


// configure the MPU to protect address range starting at 0x00000000
void init_MPU(void) {
    MPU->RBAR = 0x0U                          	// base address
                | MPU_RBAR_VALID_Msk          	// valid region
                | (7U << MPU_RBAR_REGION_Pos); 	// region #7
    MPU->RASR = (7U << MPU_RASR_SIZE_Pos)     	// 2^(7+1) bytes size
                | (0x0U << MPU_RASR_AP_Pos)   	// no-access region
                | MPU_RASR_ENABLE_Msk;        	// enable region
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk 		// enable background region
                | MPU_CTRL_ENABLE_Msk;  		// enable the MPU
    __ISB();
    __DSB();
}


int main(void)
{
    init_LED();
    init_MPU();

    for(volatile int i=0; i<1000000; i++);  // spend some time...
    
    int *ptr = NULL;            // http://nullpointer.de/
    volatile int x = *ptr;      // read/write access triggers hard fault handler

    // loop forever
    for(;;) 
        ;
    return x;	// unreachable code, main shall never return in embedded software
}
