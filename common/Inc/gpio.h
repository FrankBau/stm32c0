#ifndef __GPIO_H
#define __GPIO_H

#include <stm32c011xx.h>

// gpio_pin definitions
// currently only GPIOA and GPIOB are supported

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

// mode
#define INPUT 0
#define OUTPUT 1
#define ALTERNATE 2
#define ANALOG 3

// pull
#define NOPULL 0
#define PULLUP 1
#define PULLDOWN 2

// type
#define PUSH_PULL 0
#define OPEN_DRAIN 1

// speed
#define SPEED_LOW 0
#define SPEED_MEDIUM 1
#define SPEED_HIGH 2
#define SPEED_VERY_HIGH 3

// alternate function
#define AF0 0
#define AF1 1
#define AF2 2
#define AF3 3
#define AF4 4
#define AF5 5
#define AF6 6
#define AF7 7
#define AF8 8    
#define AF9 9
#define AF10 10
#define AF11 11
#define AF12 12
#define AF13 13
#define AF14 14
#define AF15 15

typedef struct {
  unsigned int mode : 2;    // INPUT, OUTPUT, ALTERNATE, ANALOG
  unsigned int pull : 2;    // NOPULL, PULLUP, PULLDOWN
  unsigned int type : 1;    // PUSH_PULL, OPEN_DRAIN
  unsigned int speed : 2;   // SPEED_LOW, SPEED_MEDIUM, SPEED_HIGH, SPEED_VERY_HIGH
  unsigned int af : 4;      // AF0 - AF15
} gpio_setup_t;


// works for GPIOA, GPIOB, GPIOC, GPIOD, (GPIOE), GPIOF
#define GPIO_PORT_INDEX(Pxy)   ((Pxy & 0xF0) >> 4)
#define GPIO_PORT(Pxy)   (GPIO_TypeDef*)((uintptr_t)IOPORT_BASE + (0x400UL * GPIO_PORT_INDEX(Pxy)))
#define GPIO_PIN(Pxy)    (Pxy & 0x0F)


static inline void gpio_setup(uint8_t Pxy, gpio_setup_t setup)
{
    RCC->IOPENR |= (1u << GPIO_PORT_INDEX(Pxy)); // enable clock for the port
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);

    port->MODER = (port->MODER & ~(3u << (2 * pin))) | (setup.mode << (2 * pin));
    port->OTYPER = (port->OTYPER & ~(1u << pin)) | (setup.type << pin);
    port->AFR[pin / 8] = (port->AFR[pin / 8] & ~(0xFu << (4 * (pin % 8)))) | (setup.af << (4 * (pin % 8)));
    port->OSPEEDR = (port->OSPEEDR & ~(3u << (2 * pin))) | (setup.speed << (2 * pin));
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
