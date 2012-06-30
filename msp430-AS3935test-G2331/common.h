//******************************************************************************
//   common.h MSP430G2231 and AS3935 lightning detector test
//
//   Description: just common includes
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Developed in CCS v5
//******************************************************************************
#ifndef COMMON_H
#define COMMON_H 1

#include <msp430g2231.h>
/*#include <io.h>*/
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
//#include <stdio.h>
#include <stdbool.h>

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif
#ifndef BOOL
typedef bool BOOL;
#endif

void driver_tick(void);

#endif
