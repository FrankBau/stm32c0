#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>

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


void gpio_init(void);

void gpio_input(uint8_t Pxy);

void gpio_output(uint8_t Pxy);

void gpio_opendrain(uint8_t Pxy);

void gpio_alternate(uint8_t Pxy, uint8_t function);
  
void gpio_additional(uint8_t Pxy);


void gpio_pullup(uint8_t Pxy);

void gpio_pulldown(uint8_t Pxy);


int gpio_get(uint8_t Pxy);

void gpio_set(uint8_t Pxy, int value);

void gpio_set_0(uint8_t Pxy);

void gpio_set_1(uint8_t Pxy);

void gpio_toggle(uint8_t Pxy);


#endif
