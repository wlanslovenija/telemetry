#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdbool.h>
#include "types.h"

enum gpio_mode {
	GPIO_INPUT = 0x10,
	GPIO_INPUT_PU = 0x11,
	GPIO_INPUT_PD = 0x12,
	GPIO_OUTPUT = 0x20,
	GPIO_OUTPUT_SLOW = 0x21,
	GPIO_ANALOG = 0x30
};

typedef u16 gpio_t;

void gpio_init(gpio_t pin, enum gpio_mode mode, bool value);
void gpio_set(gpio_t pin, bool value);
bool gpio_get(gpio_t pin);

#endif
