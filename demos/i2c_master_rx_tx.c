#define STM32C011xx     // define the target microcontroller
#include <stm32c0xx.h>  // include the CMSIS main header file
 
// SCL: PB7 SDA: PC14 
void init_i2c(void) {

    // see ref.man.  23.4.9 I2C master mode

    RCC->APBENR1 |= RCC_APBENR1_I2C1EN; // enable GPIOB and GPIOC clocks
    (void)RCC->APBENR1; // ensure that the last write command finished and the clock is on

    I2C1->CR1 &=~I2C_CR1_PE; // disable I2C peripheral for setup

    // default filter settings
    I2C1->CR1 &= ~I2C_CR1_ANFOFF;
    I2C1->CR1 &= ~I2C_CR1_DNF;

    I2C1->TIMINGR = 0x40000A0B; // for I2C1 clock = 12 MHz and I2C Standard Mode @ 100kHz

    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH; // enable clock stretching
    
    I2C1->CR1 |= I2C_CR1_PE; // enable I2C peripheral
    
    // Configure the Pins for I2C
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN; // enable GPIOB and GPIOC clocks
    (void)RCC->IOPENR; // ensure that the last write command finished and the clock is on

    // Set PB7 to I2C1_SCL AF mode 
    GPIOB->OTYPER = (GPIOB->OTYPER &~GPIO_OTYPER_OT7_Msk) | (1 << GPIO_OTYPER_OT7_Pos); // open-drain
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR &~GPIO_OSPEEDR_OSPEED7_Msk)|(3<<GPIO_OSPEEDR_OSPEED7_Pos);//v.high
    GPIOB->PUPDR = (GPIOB->PUPDR &~GPIO_PUPDR_PUPD6_Msk) | (1<<GPIO_PUPDR_PUPD6_Pos); // pull-up
    //GPIOB->BSRR = GPIO_BSRR_BS6; // GPIO output level - not used
    GPIOB->AFR[0] = (GPIOB->AFR[0] & ~GPIO_AFRL_AFSEL7_Msk) | (14 << GPIO_AFRL_AFSEL7_Pos); // AF14
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE7_Msk) | (2 << GPIO_MODER_MODE7_Pos);   // AF mode

    // Set PC14 to I2C1_SDA AF mode 
    GPIOC->OTYPER = (GPIOC->OTYPER &~GPIO_OTYPER_OT14_Msk) | (1 << GPIO_OTYPER_OT14_Pos); // open-drain
    GPIOC->OSPEEDR = (GPIOC->OSPEEDR &~GPIO_OSPEEDR_OSPEED14_Msk)|(3<<GPIO_OSPEEDR_OSPEED14_Pos);
    GPIOC->PUPDR = (GPIOC->PUPDR &~GPIO_PUPDR_PUPD14_Msk) | (1<<GPIO_PUPDR_PUPD14_Pos); // pull-up
    //GPIOC->BSRR = GPIO_BSRR_BS6; // GPIO output level - not used
    GPIOC->AFR[1] = (GPIOC->AFR[1] & ~GPIO_AFRH_AFSEL14_Msk) | (14 << GPIO_AFRH_AFSEL14_Pos); // AF14
    GPIOC->MODER = (GPIOC->MODER & ~GPIO_MODER_MODE14_Msk) | (2 << GPIO_MODER_MODE14_Pos);   // AF mode
}

// nbytes must be 1..255
int i2c_write(uint8_t addr, uint8_t nbytes, uint8_t *data) {

    while(I2C1->ISR & I2C_ISR_BUSY);    // wait for bus free

    // ref.man.  Figure 228. Transfer sequence flow for I2C master transmitter for N ≤ 255 byte

    I2C1->CR2 = nbytes << I2C_CR2_NBYTES_Pos; // length of the data transfer
    I2C1->CR2 |= ((addr<<1) << I2C_CR2_SADD_Pos); // beware: 7-bit slave addr expected here
    // I2C1->CR2 &= ~I2C_CR2_AUTOEND;   // 0: for no stop generation (restart)
    I2C1->CR2 |= I2C_CR2_AUTOEND;       // 1: for automatic stop generation after the last byte
    I2C1->CR2 &=~I2C_CR2_RD_WRN;        // 0: Master requests a write transfer
    I2C1->CR2 |= I2C_CR2_START;         // send start condition followed by the address sequence

    for(;;) {
        if(I2C1->ISR & I2C_ISR_TXE) {   // transmit data register empty? 
            I2C1->TXDR = *data++;       // transmit next byte, this clears TXE
            if(--nbytes==0)             // last byte transmitted?
                return 1;               // normal end of transfer
        } else if(I2C1->ISR & I2C_ISR_NACKF) {
            return 0;                   // no ACK seen on I2C bus, abort transfer
        }
    }
}

// nbytes must be 1..255
int i2c_read(uint8_t addr, uint8_t nbytes, uint8_t *data) {

    while(I2C1->ISR & I2C_ISR_BUSY);    // wait for bus free

    // ref.man.  Figure 231. Transfer sequence flow for I2C master receiver for N ≤ 255 bytes

    I2C1->CR2 = nbytes << I2C_CR2_NBYTES_Pos; // length of the data transfer
    I2C1->CR2 |= ((addr<<1) << I2C_CR2_SADD_Pos); // beware: 7-bit slave addr expected here
    // I2C1->CR2 &= ~I2C_CR2_AUTOEND;   // 0: for no stop generation (restart)
    I2C1->CR2 |= I2C_CR2_AUTOEND;       // 1: for automatic stop generation
    I2C1->CR2 |= I2C_CR2_RD_WRN;        // 1: Master requests a read transfer
    I2C1->CR2 |= I2C_CR2_START;

    for(;;) {
        if(I2C1->ISR & I2C_ISR_RXNE) {  // receive data register not empty?
            *data++ = I2C1->RXDR;       // read next byte, this clears RXNE
            if(--nbytes==0)             // last byte received?
                return 1;               // normal end of transfer
        } else if(I2C1->ISR & I2C_ISR_NACKF) {
            return 0;                   // no ACK seen on I2C bus, abort transfer
        }
    }
}

int main(void)
{
    init_i2c();

    uint8_t tx[4] = {0x5A, 0x00, 0xFF, 0xA5};
    uint8_t rx[4] = {0};

    for(;;) {
        i2c_write(0x50, sizeof(tx), tx);
        i2c_read(0x50, sizeof(rx), rx);
    }
}