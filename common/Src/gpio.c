#include <stm32c011xx.h>

#include <gpio.h>

// works for GPIOA, GPIOB, GPIOC, GPIOD, (GPIOE), GPIOF
#define GPIO_PORT(Pxy)   (GPIO_TypeDef*)(GPIOA_BASE + ((Pxy & 0xF0) << 6))

#define GPIO_PIN(Pxy)    (Pxy & 0x0F)

// currently only GPIOA and GPIOB are supported

// to keep the code small, it is assumed that the microcontroller registers
// still have their reset default values and were not changed by other code.

// enable clock for GPIOA and GPIOB ports, to be called first
void gpio_init(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
}

// set the mode of pin Pxy to mode
// mode is one of the following: 0, 1, 2, 3
void gpio_set_mode(uint8_t Pxy, int mode)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->MODER &= ~(3 << (2 * pin));       // clear pin mode bits
    port->MODER |=  (mode << (2 * pin));    // set mode
}

// set the pull-up/down of pin Pxy to pull
// pull is one of the following: 0, 1, 2
void gpio_set_pull(uint8_t Pxy, int pull)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->PUPDR &= ~(3 << (2 * pin));       // clear pin pull bits
    port->PUPDR |=  (pull << (2 * pin));    // set pull
}

// set the type of pin Pxy to type
// type is one of the following: 0, 1
void gpio_set_type(uint8_t Pxy, int type)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->OTYPER &= ~(1 << pin);            // clear pin type bit
    port->OTYPER |=  (type << pin);         // set type
}

// set the alternate function of pin Pxy to af
// af is a 4-bit value
void gpio_set_af(uint8_t Pxy, int af)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->AFR[pin / 8] &= ~(0xF << (4 * (pin % 8)));
    port->AFR[pin / 8] |= (af << (4 * (pin % 8)));
}

//////////////////////////////////////////////////////////////
// The following functions are the public API of the module //
//////////////////////////////////////////////////////////////

// configure pin Pxy as a digital input
void gpio_input(uint8_t Pxy)
{
    gpio_set_mode(Pxy, 0);
}

// configure pin as a digital output (push-pull)
void gpio_output(uint8_t Pxy)
{
    gpio_set_type(Pxy, 0);
    gpio_set_mode(Pxy, 1);
}

// configure pin as a digital open-drain output
void gpio_opendrain(uint8_t Pxy)
{
    gpio_set_type(Pxy, 1);
    gpio_set_mode(Pxy, 1);
}

// configure pin for alternate functions (see data sheet for AF)
void gpio_alternate(uint8_t Pxy, uint8_t function)
{
    gpio_set_af(Pxy, function);
    gpio_set_mode(Pxy, 2);
}

// configure pin for additional functions
// see data sheet, this is the pin reset state
void gpio_additional(uint8_t Pxy)
{
    gpio_set_mode(Pxy, 3);
}

// enable internal pull-up resistor
void gpio_pullup(uint8_t Pxy)
{
    gpio_set_pull(Pxy, 1);
}

// enable internal pull-down resistor
void gpio_pulldown(uint8_t Pxy)
{
    gpio_set_pull(Pxy, 2);
}

// get level of Pin Pxy
// returns 0 if low, 1 if high
int gpio_get(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    return 1u & (port->IDR >> pin);
}

// set the level of a digital output pin Pxy to value
// value must be 0 for low, 1 for high
void gpio_set(uint8_t Pxy, int value)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->ODR = (port->ODR & ~(1 << pin)) | (value << pin);
}

// set the level of a digital output pin Pxy to 0 (low)
void gpio_set_0(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->BRR = (1u << pin);
}

// set the level of a digital output pin Pxy to 1 (high)
void gpio_set_1(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->BSRR = (1u << pin);
}

// toggle the level of a digital output pin Pxy
void gpio_toggle(uint8_t Pxy)
{
    GPIO_TypeDef *port = GPIO_PORT(Pxy);
    int pin = GPIO_PIN(Pxy);
    port->ODR ^= (1 << pin);
}
