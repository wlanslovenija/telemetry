/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */

#include "msp430_gpio.h"
#include <stdio.h>
#include "gpio.h"
#include "types.h"
#include <msp430.h>

/* offsets from base reg - XXX hopefully these are ok for all chips */
#define INoff 0
#define OUToff 1
#define DIRoff 2
#define RENoff 7
void gpio_init(gpio_t pin, enum gpio_mode mode, bool value)
{
	int port = (pin & 0xf0) >> 4;
	pin = pin & 0xf;

	volatile u8 *base = (u8*)&P1IN;
	if (port == 1)
		base = (u8*)&P1IN;
	else if (port == 2)
		base = (u8*)&P2IN;

	if (mode & GPIO_OUTPUT) {
		if (value)
			base[OUToff] |= 1<<pin;
		else
			base[OUToff] &= ~(1<<pin);
		base[DIRoff] |= 1<<pin;
	} else if (mode & GPIO_INPUT) {
		base[DIRoff] &= ~(1<<pin);
		if (mode == GPIO_INPUT_PU) {
			base[OUToff] |= 1<<pin;
			base[RENoff] |= 1<<pin;
		} else if (mode == GPIO_INPUT_PD) {
			base[OUToff] &= ~(1<<pin);
			base[RENoff] |= 1<<pin;
		} else {
			base[RENoff] &= ~(1<<pin);
		}
	}
}

void gpio_set(gpio_t pin, bool value)
{
	int port = (pin & 0xf0) >> 4;
	pin = pin & 0xf;

	volatile u8 *base = (u8*)&P1IN;
	if (port == 1)
		base = (u8*)&P1IN;
	else if (port == 2)
		base = (u8*)&P2IN;

	if (value)
		base[OUToff] |= 1<<pin;
	else
		base[OUToff] &= ~(1<<pin);
}

bool gpio_get(gpio_t pin)
{
	int port = (pin & 0xf0) >> 4;
	pin = pin & 0xf;

	volatile u8 *base = (u8*)&P1IN;
	if (port == 1)
		base = (u8*)&P1IN;
	else if (port == 2)
		base = (u8*)&P2IN;

	return (base[INoff]>>pin) & 1;
}
