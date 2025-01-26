#define STM32C011xx
#include <stm32c0xx.h>


#define VREFINT_CAL_ADDR      ((uint16_t*)0x1FFF756AUL)     // VREFINT_CAL digital calibration value address
#define VREFINT_CAL_VREF                   ( 3000UL)        // VREFINT_CAL value in mV


#define TEMPSENSOR_CAL1_ADDR  ((uint16_t*)0x1FFF7568UL)     // TEMPSENSOR_CAL1 digital calibration value address
#define TEMPSENSOR_CAL1_TEMP               (  30UL)         // TEMPSENSOR_CAL1 value in °C


// global variables, can be watched by cortex-debug live watch or diagrammed by STM32CubeMonitor
volatile struct {	// the selected ADC channels raw digital values by increasing channel number
  uint16_t pa7;
  uint16_t joy;
  uint16_t temp;
  uint16_t vref;
  uint16_t vdda;
  uint16_t vssa;
} adc_raw_data;


volatile struct {	// derived (calculated) values in physical units
  uint16_t pa7_mV;
  uint16_t joy_mV;
  uint16_t temp_degC;
  // uint16_t vref_mV; // vref is the reference input, not a calculated result
  uint16_t vdda_mV;
  uint16_t vssa_mV;
} adc_data;


void init_ADC(void) {
  // the GPIO pins used for ADC are already in analog mode after reset
  // no need to re-config GPIO here (unless used before)
  
  // let ADC (digital block) be clocked by: SYSCLK
  RCC->CCIPR = (RCC->CCIPR &~RCC_CCIPR_ADCSEL_Msk) | (0<<RCC_CCIPR_ADCSEL_Pos); 
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;  // turn ADC clock on
  (void)RCC->APBENR2; // read back to make sure that clock is on
  ADC1->CR |= ADC_CR_ADVREGEN; // power up ADC voltage regulator 
  // wait  t_ADCVREG_STUP (ADC voltage regulator start-up time), 
  for(volatile int i=0; i<12*20; ++i); // min 20 µs see data sheet

  // do self calibration
  ADC1->CR |= ADC_CR_ADCAL;
  while(ADC1->CR & ADC_CR_ADCAL); // wait for calibration to finish
  uint8_t calibration_factor = ADC1->DR;

  ADC1->CFGR1 = 0;  // default config after reset
  ADC1->CFGR2 = 0;  // default config after reset
  ADC1->SMPR = 0;   // sampling time register, default after reset
  // "enable the ADC" procedure from RM0490 Rev 3:
  ADC1->ISR |= ADC_ISR_ADRDY; //  Clear the ADRDY bit in ADC_ISR register 
  ADC1->CR |= ADC_CR_ADEN;    //  Set ADEN = 1 in the ADC_CR register.
  while(!(ADC1->ISR & ADC_ISR_ADRDY)); //  Wait until ADRDY = 1 in the ADC_ISR register

  ADC1->CALFACT = calibration_factor;

  // above: CHSELRMOD = 0 in ADC_CFGR1, so every channel has a bit. set bit to activate that channel
  // ADC1->CHSELR = ADC_CHSELR_CHSEL8; // select channel ADC_IN8 which is PA8 connected to joystick
  ADC1->CHSELR = 
      ADC_CHSELR_CHSEL7   // select channel ADC_IN7 which is PA7 freely available
  |   ADC_CHSELR_CHSEL8   // select channel ADC_IN8 which is PA8 connected to joystick
  |   ADC_CHSELR_CHSEL9   // temperature sensor V_sense 
  |   ADC_CHSELR_CHSEL10  // internal reference voltage V_refint 
  |   ADC_CHSELR_CHSEL15  // Vdda 
  |   ADC_CHSELR_CHSEL16  // Vssa 
  ;
  while(!(ADC1->ISR & ADC_ISR_CCRDY)); // wait until channel configuration update is applied

  // At the end of ADC initialization, the internal temp. sensor must be woken up from power-down:

  ADC->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN; // wake up temp. + vrefint blocks from power down mode
  for(volatile int i=0; i<12*15; ++i); // 15µs wait for stabilization, see data sheet
}


void init_DMA_ADC(void) {
    RCC->AHBENR |= RCC_AHBENR_DMA1EN; // enable peripheral clock
    (void)RCC->AHBENR; // read back to make sure that clock is on

    // route peripheral DMA request to DMA channel
    // Table 34: DMAMUX adc_dma == 5
    // caution: DMAMUX1_Channel0 is for DMA1_Channel1 and so on!
    DMAMUX1_Channel0->CCR = 5 << DMAMUX_CxCR_DMAREQ_ID_Pos;

    DMA1_Channel1->CCR &= ~DMA_CCR_EN;  // disable DMA channel for setup
    DMA1->IFCR = DMA_IFCR_CGIF1;          // clear all (HT, TC, TE) flags for DMA channel 1
    
    DMA1_Channel1->CPAR = (uint32_t)(&ADC1->DR);
    DMA1_Channel1->CMAR = (uint32_t)&adc_raw_data;
    DMA1_Channel1->CNDTR = sizeof(adc_raw_data) / sizeof(uint16_t);
    DMA1_Channel1->CCR = 
        0 << DMA_CCR_MEM2MEM_Pos    // MEM2MEM 1: memory-to-memory mode
    |   0 << DMA_CCR_PL_Pos         // PL priority level 0: low.. 3: very high
    |   1 << DMA_CCR_MSIZE_Pos      // MSIZE 0: 8-bit 1: 16-bit 2: 32-bit
    |   1 << DMA_CCR_PSIZE_Pos      // PSIZE 0: 8-bit 1: 16-bit 2: 32-bit
    |   1 << DMA_CCR_MINC_Pos       // MINC memory increment mode   1: enable
    |   0 << DMA_CCR_PINC_Pos       // PINC peripheral increment mode   1: enable
    |   1 << DMA_CCR_CIRC_Pos       // CIRC 0 : normal mode 1: circular mode
    |   0 << DMA_CCR_DIR_Pos        // DIR 0: read from peripheral,     1: memory
    |   0 << DMA_CCR_TEIE_Pos       // TEIE transfer error interrupt    1: enable
    |   0 << DMA_CCR_HTIE_Pos       // HTIE half transfer interrupt     1: enable
    |   0 << DMA_CCR_TCIE_Pos       // TCIE transfer complete interrupt     1: enable
    |   1 << DMA_CCR_EN_Pos         // EN enable DMA channel
    ;
}


int main(void)
{
  init_ADC();
  init_DMA_ADC();

  // sampling time set to 7 (max): 160.5 ADC clock cycles. 
  // internal temperature sensor needs at least 5 µs sampling time, see data sheet
  ADC1->SMPR = (ADC1->SMPR &~ADC_SMPR_SMP1_Msk) | (7 << ADC_SMPR_SMP1_Pos);

  ADC1->CFGR1 |= ADC_CFGR1_CONT;  	// 1: continuos conversion mode
  ADC1->CFGR1 |= ADC_CFGR1_DMACFG;	// 1: DMA circular mode selected
  ADC1->CFGR1 |= ADC_CFGR1_DMAEN; 	// enable DMA use in ADC
  ADC1->CR |= ADC_CR_ADSTART;     	// start ADC conversion sequence(s)

  for(;;) {
    // see reference manual: Calculating the actual VDDA
    int VREFINT_CAL = *VREFINT_CAL_ADDR;  // calibration value (engineering bytes) 
    adc_data.vdda_mV = (VREFINT_CAL_VREF * VREFINT_CAL) / adc_raw_data.vref;

    // other voltage measurements are proportional to Vdda:
    adc_data.pa7_mV   = (adc_data.vdda_mV * adc_raw_data.pa7 ) / 4095;
    adc_data.joy_mV   = (adc_data.vdda_mV * adc_raw_data.joy ) / 4095;
    adc_data.vssa_mV  = (adc_data.vdda_mV * adc_raw_data.vssa) / 4095;

    // Reading the temperature
    int TS_CAL1 = *TEMPSENSOR_CAL1_ADDR;  // calibration value (engineering bytes)
    int t_meas_mV = adc_raw_data.temp * adc_data.vdda_mV / 4095;
    int t_calib_mV = TS_CAL1 * VREFINT_CAL_VREF / 4095;
    int Avg_Slope = 2530;   // data sheet: average slope from VSENSE voltage, but in µV/°C

    adc_data.temp_degC = TEMPSENSOR_CAL1_TEMP + (t_meas_mV-t_calib_mV) * 1000 / Avg_Slope;
  }
}
