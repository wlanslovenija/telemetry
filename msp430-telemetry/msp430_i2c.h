#ifndef _MSP430_I2C_H_
#define _MSP430_I2C_H_

#include "types.h"

int i2c_init(u32 clock);
int i2c_write_read(u8 addr, const u8 *txbuf, int txlen, u8 *rxbuf, int rxlen);

#endif
