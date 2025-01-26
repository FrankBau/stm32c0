#ifndef __GPIO_H
#define __GPIO_H

#include <stm32c011xx.h>

#define  PA0 0x00
#define  PA1 0x01
#define  PA2 0x02
#define  PA3 0x03
#define  PA4 0x04
#define  PA5 0x05
#define  PA6 0x06
#define  PA7 0x07
#define  PA8 0x08
#define  PA9 0x09
#define PA10 0x0A
#define PA11 0x0B
#define PA12 0x0C
#define PA13 0x0D
#define PA14 0x0E
#define PA15 0x0F

#define  PB0 0x10
#define  PB1 0x11
#define  PB2 0x12
#define  PB3 0x13
#define  PB4 0x14
#define  PB5 0x15
#define  PB6 0x16
#define  PB7 0x17
#define  PB8 0x18
#define  PB9 0x19
#define PB10 0x1A
#define PB11 0x1B
#define PB12 0x1C
#define PB13 0x1D
#define PB14 0x1E
#define PB15 0x1F

// works for GPIOA, GPIOB, GPIOC, GPIOD, (GPIOE), GPIOF
#define GPIO_PORT(Pxy)   (GPIO_TypeDef*)(GPIOA_BASE + ((Pxy & 0xF0) << 6))

#define GPIO_PIN(Pxy)    (Pxy & 0x0F)

// currently only GPIOA and GPIOB are supported

// to keep the code small, it is assumed that the microcontroller registers
// still have their reset default values and were not changed by other code.

// enable clock for GPIOA and GPIOB ports
// to be called first
static inline void gpio_init(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
}

// set the mode of pin Pxy to mode
// mode is one of the following: 0: input, 1: output, 2: alternate, 3: additional
static inline void gpio_set_mode(uint8_t Pxy, int mode)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->MODER = (port->MODER & ~(3u << (2 * pin))) | (mode << (2 * pin));
}

// set the pull-up/down of pin Pxy to pull
// pull is one of the following: 0: no pull, 1: pull-up, 2: pull-down
static inline void gpio_set_pull(uint8_t Pxy, int pull)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->PUPDR = (port->PUPDR & ~(3u << (2 * pin))) | (pull << (2 * pin));
}

// set the output type of pin Pxy to type
// type is one of the following: 0: push-pull, 1: open-drain
static inline void gpio_set_type(uint8_t Pxy, int type)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->OTYPER = (port->OTYPER & ~(1u << pin)) | (type << pin);
}

// set the alternate function of pin Pxy to af
// af is a 4-bit value, see data sheet tables
static inline void gpio_set_af(uint8_t Pxy, int af)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->AFR[pin / 8] = (port->AFR[pin / 8] & ~(0xFu << (4 * (pin % 8)))) | (af << (4 * (pin % 8)));
}

// set the output driver speed of pin Pxy to speed
// speed is one of the following: 0: slow, 1, 2, 3: very high
static inline void gpio_set_speed(uint8_t Pxy, int speed)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->OSPEEDR = (port->OSPEEDR & ~(3u << (2 * pin))) | (speed << (2 * pin));
}

// get the input level of pin Pxy
// returns 0 (low) or 1 (high)
static inline int gpio_get(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    return 1u & (port->IDR >> pin);
}

// set the output of pin Pxy to value
// value is 0 (low) or 1 (high)
static inline void gpio_set(uint8_t Pxy, int value)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->ODR = (port->ODR & ~(1 << pin)) | (value << pin);
}

// set the output of pin Pxy to 0 (low)
static inline void gpio_set_0(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->BRR = (1u << pin);
}

// set the output of pin Pxy to 1 (high)
static inline void gpio_set_1(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->BSRR = (1u << pin);
}

// toggle the output of pin Pxy
// 0 (low) <--> 1 (high)
static inline void gpio_toggle(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->ODR ^= (1 << pin);
}

#endif
