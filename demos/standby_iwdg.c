// use standby power-saving mode with periodic IWDG wakeup
// standby current measured: 7.5 uA @ 2.4V

#define STM32C011xx
#include <stm32c0xx.h>

// Standby mode
// VCORE domain is powered off and the SRAM and register contents lost, 
// except PWR control register 3 (PWR_CR3) and PWR backup x register (PWR_BKPxR).
// All clocks in the VCORE domain are stopped and the HSI48 and HSE oscillators disabled.
// The IWDG and the LSI oscillator can be kept running.
// The event of exiting Standby mode enables the HSI48 oscillator, selects HSISYS as
// system clock and sets its prescaler division factor to four (HSIDIV[2:0]=010).
void Standby(void)
{
	// Configure wake-up features
	// ...
	PWR->SCR |= PWR_SCR_CWUF | PWR_SCR_CSBF; // clear all wakeup flags, must be done last
											 // Requires three extra APB clock cycles, compared to standard APB access.

	SCB->SCR |= (1 << SCB_SCR_SLEEPDEEP_Pos); // enable CPU core deep sleep

	PWR->CR1 = (PWR->CR1 & ~PWR_CR1_LPMS_Msk) | (3 << PWR_CR1_LPMS_Pos); // LPMS[2:0] – select standby mode
	PWR->CR1 |= PWR_CR1_FPD_STOP_Msk;									 // Flash power down mode during stop
	PWR->CR1 |= PWR_CR1_FPD_SLP_Msk;									 // Flash power down mode during sleep
	(void)PWR->CR1;														 // ensure completion of previous write

	DBG->CR &= ~DBG_CR_DBG_STANDBY; // disable debug in standby mode

	// Enter low-power mode
	for (;;)
	{
		__DSB();
		__WFI();
	}
}

// Configuring the IWDG (when the window option is disabled)
void init_IWDG(void)
{
	// IWDG is independently clocked by the 32 kHz LSI clock, no need to switch a clock on

	IWDG->KR = 0xCCCC; 	// key register: enable the watchdog
	IWDG->KR = 0x5555; 	// key register: unprotect register write access

	IWDG->PR = 7; 		// maximum prescaler of 256. -> IWDG counts at 125 Hz
	IWDG->RLR = 625; 	// reload register. IWDG expires after 5 seconds
	while (IWDG->SR); 	// wait until the reload value is updated
	
	IWDG->KR = 0xAAAA; 	// key register: refresh the watchdog
}


// LED pin PB6 setup
// note that the LED low active: setting PB6 to high will turn off the LED
void init_LED(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;  // enable GPIOB port by switching its clock on
    (void)RCC->IOPENR;                  // make sure that the clock is on by now

    // set the pin PB6 to general purpose **output** mode (which is mode 1)
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE6_Msk) | (1 << GPIO_MODER_MODE6_Pos);
}


void delay_ms(uint32_t milliseconds)
{
	volatile uint32_t count;
	for (count = 0; count < milliseconds; count++)
	{
		__asm volatile(
			"ldr r0, =4000\n" // Load the number of iterations (adjust this value based on clock speed)
			"1: sub r0, #1\n" // Subtract 1 from r0
			"bne 1b\n"		  // Branch to label 1 backwards if r0 is not zero
			:				  // No output operands
			:				  // No input operands
			: "r0"			  // Clobbered register
		);
	}
}

void blink(int n, int ms)
{
	for (int i = 0; i < 2*n; ++i)
	{
		GPIOB->ODR ^= ~GPIO_ODR_OD3; // toggle LED
		delay_ms(ms);
	}
}

int main(void)
{
	// Upon power-on reset or upon wake-up from Standby, the HCLK clock frequency is automatically set to 12 MHz
	init_LED();

	RCC->APBENR1 |= RCC_APBENR1_PWREN; // enable the power interface clock
	(void)RCC->APBENR1;				   // read back the register to make sure that clock is now on

	// check if the reset was caused by the IWDG
	if(RCC->CSR2 & RCC_CSR2_IWDGRSTF) {
		int n = PWR->BKP0R;				// read the backup register which survived the standby mode
		blink(n, 200);
		PWR->BKP0R = (n + 1) % 5;		// increment the backup register modulo 5
	} else {
		blink(3, 1000);
		PWR->BKP0R = 3;					// set the backup register initial value
	}
	RCC->CSR2 |= RCC_CSR2_RMVF; // remove (clear) all reset flags

	delay_ms(1000);				 // wait to help debugging

	init_IWDG(); // for periodic wakeup

	// de-init GPIOs and peripherals

	// RCC->IOPSMENR = 0;
	// RCC->AHBSMENR = 0;
	// RCC->APBSMENR1 = 0;
	// RCC->APBSMENR2 = 0;

	// RCC->APBENR1 = 0;
	// RCC->APBENR2 = 0;

	// this is recommended fort all ports, but kills the SWD debug interface
	// GPIOA->MODER = 0xFFFFFFFF; // set all pins to analog mode
	// RCC->IOPENR = 0; // switch off all GPIO clocks

	Standby();
	// unreachable code

	/* Loop forever */
	for (;;)
		;
}
