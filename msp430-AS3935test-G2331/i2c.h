//   i2c.h MSP430G2231 and AS3935 lightning detector test
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Based on:  TI, D.Dong MSP430G2xx3 examples
//   Developed in CCS v5
//******************************************************************************
#ifndef I2C_H
#define I2C_H 1

#include "common.h"

#define SDA     BIT7 //P1.7
#define SCL     BIT6 //P1.6
#define LED     BIT0 //P1.0
#define ACK     0x00 // ACK
#define NACK    0xFF // nACK

void i2c_init(void);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
uint8_t i2c_write8(uint8_t c);
uint8_t i2c_read8(uint8_t acknack);

#endif

