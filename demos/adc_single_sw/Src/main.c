#define STM32C011xx
#include <stm32c0xx.h>


// global variables, can be watched by cortex-debug live watch or diagrammed by STM32CubeMonitor
uint32_t adc_data_raw;  // raw 12-bit ADC result
uint32_t adc_data_mV;   // result in millivolts


// measure voltage at PA8 pin which is connected to the joystick
void init_ADC(void) {
    // the GPIO pins used for ADC are already in analog mode after reset
    // no need to re-config GPIO here (unless used before)

    // let ADC (digital block) be clocked by: SYSCLK
    RCC->CCIPR = (RCC->CCIPR &~RCC_CCIPR_ADCSEL_Msk) | (0<<RCC_CCIPR_ADCSEL_Pos); 
    RCC->APBENR2 |= RCC_APBENR2_ADCEN;  // turn ADC clock on
    (void)RCC->APBENR2; // read back to make sure that clock is on
    ADC1->CR |= ADC_CR_ADVREGEN; // power up ADC voltage regulator 
    // wait  t_ADCVREG_STUP (ADC voltage regulator start-up time), 
    for(volatile int i=0; i<12*20; ++i); // min 20 µs see data sheet, 12 MHz clock assumed

    // do self-calibration
    ADC1->CR |= ADC_CR_ADCAL;
    while(ADC1->CR & ADC_CR_ADCAL); // wait for calibration to finish
    uint8_t calibration_factor = ADC1->DR;

    ADC1->CFGR1 = 0; 	// default config after reset
    ADC1->CFGR2 = 0; 	// default config after reset
    ADC1->SMPR  = 0; 	// sampling time register, default after reset
    // "enable the ADC" procedure from RM0490 Rev 3:
    ADC1->ISR |= ADC_ISR_ADRDY; //  Clear the ADRDY bit in ADC_ISR register 
    ADC1->CR |= ADC_CR_ADEN;    //  Set ADEN = 1 in the ADC_CR register.
    while(!(ADC1->ISR & ADC_ISR_ADRDY)); //  Wait until ADRDY = 1 in the ADC_ISR register

    ADC1->CALFACT = calibration_factor;

    // above: CHSELRMOD = 0 in ADC_CFGR1, so every channel has a bit. set bit to activate that channel
    ADC1->CHSELR = ADC_CHSELR_CHSEL8; // select channel ADC_IN8 which is PA8 connected to joystick
    while(!(ADC1->ISR & ADC_ISR_CCRDY)); // wait until channel configuration update is applied
}

int main(void)
{
    init_ADC();

    uint32_t Vdda_mV = 3300;    // there are more precise ways to estimate Vdda using Vrefint

    // voltage will vary when moving/pressing the joysick in different directions
    // the joystick is wired to a voltage divider, so the voltage at PA8 will be between 0V and Vdda
    // the ADC will convert this voltage to a 12-bit digital value (0-4095)
    
    while(1) {
        ADC1->CR |= ADC_CR_ADSTART;                     // start ADC conversion
        while(!(ADC1->ISR & ADC_ISR_EOC));              // wait for end of conversion
        adc_data_raw = ADC1->DR;                        // conversion done. store result
        adc_data_mV = (adc_data_raw * Vdda_mV) / 4095;  // Vdda == 4095 digital reading
    }
}
