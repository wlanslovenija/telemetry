//******************************************************************************
//   frequencyCounter.h MSP430G2553 frequency counter
//
//   Description: Frequency counter for use with an external or internal clock sources.
// 	 Implements ltos() function for converting unsigned long to ASCII
//
//   Musti
//   wlan slovenia http://dev.wlan-si.net
//   May 2012
//   Based on: 	Frequency Counter using Launchpad & Nokia 5110 LCD by oPossum
//******************************************************************************

#ifndef FREQUENCYCOUNTER_H_
#define FREQUENCYCOUNTER_H_

void set_input(char clock_input);			//internal use only
void set_gate(unsigned long f);				//internal use only
unsigned long measureFrequency(char input); //use for measuring frequency
void ltos(unsigned long val, char *str);	//convert long to ASCII


#endif /* FREQUENCYCOUNTER_H_ */
