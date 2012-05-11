#ifndef _BITBANG_1W_H_
#define _BITBANG_1W_H_

#include <1w.h>

struct bitbang_1w_data {
	int pin;
};

int bitbang_1w_register(struct w1_master *master);

#endif
