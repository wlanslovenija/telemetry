/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL, see file LICENSE */
#include <bitbang_1w.h>
#include <1w.h>
#include <gpio.h>
#include <board.h>
#include <lock.h>
#include <errno.h>


/*
 * One wire over GPIO. Supports parasitic nodes.
 * The only thing you need to do is put a 1k+ pull-up on 1-wire line.
 */


static int bitbang_1w_reset(struct w1_master *master)
{
	struct bitbang_1w_data *chip = master->priv;
	int pin = chip->pin;

	gpio_init(pin, GPIO_OUTPUT, 0);
	udelay(500);
	gpio_init(pin, GPIO_INPUT, 0);
	/* could read for presense here */
	udelay(500);

	return 0;
}

/*
 * here's how I did it:
 * write 1/read: out 0, 2us, in (1), 10us, read, 50us (total of 62us)
 * write 0:      out 0, 60us, in (1),             2us (total of 62us)
 */
static int bitbang_1w_single_bit(int pin, int bit)
{
	/* write 0 */
	if (bit == 0) {
		gpio_init(pin, GPIO_OUTPUT, 0);
		udelay(60);
		gpio_init(pin, GPIO_INPUT, 0);
		udelay(2);
		return 0;
	}

	/* write 1 / read */
	gpio_init(pin, GPIO_OUTPUT, 0);
	udelay(2);
	gpio_init(pin, GPIO_INPUT, 0);
	udelay(10);
	bit = gpio_get(pin);
	udelay(50);
	return bit;
}

static int bitbang_1w_write(struct w1_master *master, u8 data)
{
	struct bitbang_1w_data *chip = master->priv;
	int pin = chip->pin;
	int i;
	struct lock l;

	lock(&l);
	for (i=0; i<8; i++) {
		bitbang_1w_single_bit(pin, data&1);
		data >>= 1;
	}
	unlock(&l);

	return 0;
}

static int bitbang_1w_read(struct w1_master *master)
{
	struct bitbang_1w_data *chip = master->priv;
	int pin = chip->pin;
	u8 data = 0;
	int i;
	struct lock l;

	lock(&l);
	for (i=0; i<8; i++) {
		data |= bitbang_1w_single_bit(pin, 1) << i;
	}
	unlock(&l);

	return data;
}

/* dir - 0/1 direction to take in case of device conflict */
static int bitbang_1w_triplet(struct w1_master *master, int dir)
{
	struct bitbang_1w_data *chip = master->priv;
	int pin = chip->pin;
	struct lock l;

	lock(&l);
	int bit1 = bitbang_1w_single_bit(pin, 1);
	int bit2 = bitbang_1w_single_bit(pin, 1);

	if (bit1 ^ bit2)
		bitbang_1w_single_bit(pin, bit1);
	else
		bitbang_1w_single_bit(pin, dir);
	unlock(&l);

	/* return value:
	 * bit 0: path taken
	 * bit 1: conflict?
	 */

	/* error, no device pulled the bit low */
	if (bit1 == 1 && bit2 == 1)
		return -ENODEV;

	/* conflict */
	if (bit1 == 0 && bit2 == 0)
		return 2 | dir;
	return bit1;
}

int bitbang_1w_register(struct w1_master *master)
{
	struct bitbang_1w_data *chip = master->priv;
	int pin = chip->pin;

	gpio_init(pin, GPIO_INPUT, 0);

	master->w1_reset = bitbang_1w_reset;
	master->w1_triplet = bitbang_1w_triplet;
	master->w1_write = bitbang_1w_write;
	master->w1_read = bitbang_1w_read;

	return 0;
}

