//******************************************************************************
//   console.h MSP430G2231 and AS3935 lightning detector test
//
//   Description: console library, for sending formated ASCII text
//
//   Based on examples available online.
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Developed in CCS v5
//******************************************************************************
#ifndef CONSOLE_H
#define CONSOLE_H 1

#include "common.h"

void console_init(void);

void console_putc(uint8_t c);
void console_newline(void);
void console_puts(const uint8_t *str);
void console_putsln(const uint8_t *str);
void console_puti(uint8_t *str);
void console_puth(const uint8_t *str,uint8_t count);
void cuits( uint16_t value, uint8_t* buffer);

#endif
