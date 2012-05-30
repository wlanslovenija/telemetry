/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL */

// XXX this is beta, at least one known bug: i2c_write_read() shouldn't be
// called without ~1ms delay between them

#include "msp430_i2c.h"
#include <msp430.h>
#include <stdio.h>
#include <errno.h>
#include "types.h"

// with help of: http://e2e.ti.com/support/microcontrollers/msp43016-bit_ultra-low_power_mcus/f/166/t/144869.aspx

#define SDA_PIN 0x80
#define SCL_PIN 0x40


#define EFAULT 22

int i2c_init(u32 clock)
{
//	printf("%s\n", __func__);

	UCB0CTL1 = UCSWRST;

	/* NOTE: pins need to be INPUT, datasheet doesn't mention this,
	 * resistor doesn't seem to matter */
	P1DIR &= ~(SDA_PIN | SCL_PIN);
	P1REN &= ~(SDA_PIN | SCL_PIN);

	/* set pins to USCI B0 */
	P1SEL |= SDA_PIN | SCL_PIN;
	P1SEL2 |= SDA_PIN | SCL_PIN;

	/* set clock */
	int br = CONFIG_HZ/clock;
	UCB0BR0 = br % 256;
	UCB0BR1 = br / 256;

	/* i2c master */
	UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
	UCB0CTL1 = UCSSEL_2 | UCSWRST;
	UCB0CTL1 &= ~UCSWRST;

#if 0
	printf("%s: stat 0x%02x\n", __func__, UCB0STAT);
	printf("%s: ctl1 0x%02x\n", __func__, UCB0CTL1);
	printf("%s: ifg2 0x%02x\n", __func__, IFG2);
#endif

	if (UCB0STAT & UCBBUSY) {
		printf("%s: error, i2c sda low?\n", __func__);
		return -EFAULT;
	}

#if 0
	int i;
	for (i=0x48; i<=0x48; i++) {
		printf("---\n");
		u8 txbuf[5];
		u8 rxbuf[5];
#if 0
		if (i2c_write_read(i, 0, 0, 0, 0) == 0)
			printf("%s: w 0x%x\n", __func__, i);
		if (i2c_write_read(i, NULL, 0, rxbuf, 2) != 0)
			printf("%s: %i error\n", __func__, __LINE__);
#endif

		txbuf[0] = 0xac;
		txbuf[1] = 0x00;
		if (i2c_write_read(i, txbuf, 2, 0, 0) != 0)
			printf("%s: %i error\n", __func__, __LINE__);

#if 0
		txbuf[0] = 0xee;
		if (i2c_write_read(i, txbuf, 1, 0, 0) != 0)
			printf("%s: %i error\n", __func__, __LINE__);
#endif
		txbuf[0] = 0xaa;
		if (i2c_write_read(i, txbuf, 1, rxbuf, 2) != 0)
			printf("%s: %i error\n", __func__, __LINE__);
		printf("%s: temp: %i.%02i\n", __func__, rxbuf[0], rxbuf[1]*100/256);
	}
#endif

	return 0;
}


u8 fail[12];

int i2c_write_read(u8 addr, const u8 *txbuf, int txlen, u8 *rxbuf, int rxlen)
{
	int err = 0;

//	UCB0STAT ~= UCNACKIFG;
	fail[0] = UCB0CTL1;
	fail[1] = UCB0STAT;
	fail[2]= IFG2;
	// XXX around 1ms delay is required here, or it fails at STT clear for restart */
//	if (txlen == 1 && rxlen == 2) printf("%s: ctl1:0x%02x, stat:0x%02x, ifg:0x%02x\n", __func__, UCB0CTL1, UCB0STAT, IFG2);
	fail[8] = UCB0CTL1;
	fail[9] = UCB0STAT;
	fail[10]= IFG2;
//	printf("%s: ctl1:0x%02x, stat:0x%02x, ifg:0x%02x\n", __func__, UCB0CTL1, UCB0STAT, IFG2);
	/* discard any possibly previously read data */
//	volatile int tmp = UCB0RXBUF;
//	printf("%s: %02x, w:%i, r:%i\n", __func__, addr, txlen, rxlen);

	if (UCB0STAT & UCBBUSY) {
//		printf("%s: ctl1:0x%02x, stat:0x%02x, ifg:0x%02x\n", __func__, UCB0CTL1, UCB0STAT, IFG2);
		return -EFAULT;
	}

	UCB0I2CSA = addr;

	/* send start */
	if (txlen) {
		UCB0CTL1 |= UCTR | UCTXSTT;
//		printf("%s: tx start\n", __func__);

		int i;
		for (i=0; i<txlen; i++) {

			do {
				if (UCB0STAT & (UCNACKIFG | UCALIFG)) {
					err = -EFAULT;
//					printf("%s: nack or arbitration lost\n", __func__);
//					printf("%s: ctl1:0x%02x, stat:0x%02x, ifg:0x%02x\n", __func__, UCB0CTL1, UCB0STAT, IFG2);
					goto nack;
				}
			} while ((IFG2 & UCB0TXIFG) == 0);

			UCB0TXBUF = txbuf[i];
//			printf("%s: tx %02x\n", __func__, txbuf[i]);

		}

		/* last byte NACKed is ok */
		while((IFG2 & UCB0TXIFG) == 0 && (UCB0STAT & UCNACKIFG) == 0)
			;
	}

	/* 0-byte read or write */
	if (!txlen && !rxlen) {
		/* default to write scanning */

		if (rxbuf)
			UCB0CTL1 &= ~UCTR;
		else
			UCB0CTL1 |= UCTR;
		UCB0CTL1 |= UCTXSTT | UCTXSTP;

		while (UCB0CTL1 & UCTXSTP)
			;

		if (UCB0STAT & UCNACKIFG)
			return -ENODEV;
		return 0;
	}

	/* start or repeated start */
	if (rxlen) {
	fail[4] = UCB0CTL1;
	fail[5] = UCB0STAT;
	fail[6]= IFG2;
		UCB0CTL1 &= ~UCTR;
		UCB0CTL1 |= UCTXSTT;

//		printf("%s: rx start\n", __func__);
		/* restart */
		if (txlen)
			IFG2 &= ~UCB0TXIFG;

		while (UCB0CTL1 & UCTXSTT) {
			if (UCB0STAT & UCALIFG) {
				err = -EFAULT;
//				printf("%s: arbitration lost\n", __func__);
				goto nack;
			}
		}
		if (UCB0STAT & UCNACKIFG) {
			err = -ENODEV;
//			printf("%s: nack\n", __func__);
			goto nack;
		}

		int i;
		for (i=0; i<rxlen; i++) {
			/* send stop, not ACK. in theory this could be done
			 * after flag checking, in practice, we have to do it
			 * before, or another byte is received from slave */
			if (i == rxlen-1)
				UCB0CTL1 |= UCTXSTP;

			do {
				if (UCB0STAT & (UCNACKIFG | UCALIFG)) {
					err = -EFAULT;
//					printf("%s: ctl1:0x%02x, stat:0x%02x, ifg:0x%02x\n", __func__, UCB0CTL1, UCB0STAT, IFG2);
					goto nack;
				}
			} while ((IFG2 & UCB0RXIFG) == 0);

			rxbuf[i] = UCB0RXBUF;
//			printf("%s: rx recv %02x\n", __func__, rxbuf[i]);
		}
		goto nostop;
	}

 nack:
	UCB0CTL1 |= UCTXSTP;
 nostop:
	IFG2 &= ~UCB0TXIFG;
//	IFG2 &= ~UCB0RXIFG; // maybe
	while (UCB0CTL1 & UCTXSTP)
		;

//	printf("%s: ctl1:0x%02x, stat:0x%02x, ifg:0x%02x\n", __func__, UCB0CTL1, UCB0STAT, IFG2);
//	printf("%s: err:%i\n", __func__, err);

	return err;
}
