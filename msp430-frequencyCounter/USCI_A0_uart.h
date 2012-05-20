//******************************************************************************
//   USCI_A0_uart.h MSP430G2553 simple UART output
//
//   Description: Basic UART communication with 16MHz clock.
//	 Additional formating functions for easier use. RX has 16 char buffer
//
//   Musti
//   wlan slovenia http://dev.wlan-si.net
//   May 2012
//   Based on: 	TI, D.Dong MSP430G2xx3 examples
//******************************************************************************
#include  "common.h"

#ifndef USCI_A0_UART_H_
#define USCI_A0_UART_H_

void uart_init(void);					//init the uart interface
void puts(char *str);					//send string
void putsln(char *str);					//send string and terminate line
void putc(uint8_t data);				//send character
void newline(void);						//send linebreak
bool gets(uint8_t data[]);				//get string terminated with linebreak - max length 16 char


#endif /* USCI_A0_UART_H_ */
