//******************************************************************************
//   frequencyCounter.c MSP430G2553 frequency counter
//
//   Description: Frequency counter for use with an external or internal clock sources.
// 	 Implements ltos() function for converting unsigned long to ASCII
//
//   Musti
//   wlan slovenia http://dev.wlan-si.net
//   May 2012
//   Based on: 	Frequency Counter using Launchpad & Nokia 5110 LCD by oPossum
//******************************************************************************

#include  "common.h"
#include  "USCI_A0_uart.h"

#define DEBUG									//uncomment for debug outputs

//base 10 long to ASCII conversion method, works for unsigned long, can cast integers to it as well
void ltos(unsigned long val, char *str)
{
  long temploc = 0;
  long digit = 0;
  long strloc = 0;
  char tempstr[10]; 							//32-bit number can be at most 10 ASCII digits;

  do{
	digit = val % 10;
    tempstr[temploc++] = digit + '0';
    val /= 10;
  } while (val > 0);
  	  	  	  	  	  	  	  	  	  	  	  	// reverse the digits back into the output string
  while(temploc>0)
  str[strloc++] = tempstr[--temploc];
  str[strloc]=0;
}

void set_input(char clock_input)				//state machine for choosing clock sources
{
	switch(clock_input) {
		default:
			clock_input = 0;
		case 0:
			TACTL = TASSEL_2;
			#ifdef DEBUG
			puts("Internal 16MHz ");			//internal SMCLK - 16MHz clock;
			#endif
			break;
		case 1:
			TACTL = TASSEL_0;
			#ifdef DEBUG
			puts("Clock In P1.0 ");			//Clock on P1.0 - TACLK
			#endif
			break;
		case 2:                                 // This should always show 32768
			TACTL = TASSEL_1;                   //  Something is very wrong if it doesn't
			#ifdef DEBUG
			putsln("Internal 32kHz ");			// ACLK
			#endif
			break;
	}
}

void set_gate(unsigned long f)					//how long clock cycles are counted
{
	if(WDTCTL & WDTIS0) {                       // 250 ms gate currently in use
		if(f < 800000) {                        // Switch to 1 s gate if frequency is below 800 kHz
			#ifdef DEBUG
			putsln("1 Second Gate");
			#endif
			WDTCTL = WDTPW | WDTTMSEL | WDTSSEL;
												//WDTSSEL sets ACLK as timer source, ACLK/32768
		}
	} else {                                    // 1 s gate currently in use
		if(f > 900000) {                        // Switch to 250 ms gate if frequency above 900 kHz
			#ifdef DEBUG
			putsln(" 250 ms Gate ");
			#endif
			WDTCTL = WDTPW | WDTTMSEL | WDTSSEL | WDTIS0;
												//WDTSSEL sets ACLK as timer source, WDTIS0 - ACLK/8192
		}
	}
}

unsigned long measureFrequency(char input)
{
	unsigned long freq = 12345678L;             // Measured frequency
	bool done=FALSE;							// Control variable for valid output
	char d=0;									//for loop variable

	P1SEL |= BIT0;                              // Use P1.0 as TimerA input
	P1SEL2 &= ~BIT0;                            //
	P1DIR &= ~BIT0;                             //
	P1OUT &= ~BIT0;                             // Enable pull down resistor to reduce stray counts
	P1REN |= BIT0;                              //
												//
	WDTCTL = WDTPW | WDTTMSEL | WDTCNTCL | WDTSSEL | WDTIS0; // Use WDT as interval timer
												// Default to 250 ms gate so that initial call to set_gate()
												//  will switch to 1 s gate
	set_input(input);                          	// Set input
	set_gate(0);                                // Set gate time
												//
	for(d=0;d<2;d++){							//measure twice to get the gate time right
		freq = 0;                               // Clear frequency
		TACTL |= TACLR;                         // Clear TimerA
												//
		IFG1 &= ~WDTIFG;                        // Wait for WDT period to begin
		while(!(IFG1 & WDTIFG));                //
												//
		TACTL |= MC_2;                          // Start counting - TimerA continuous mode
												//
		IFG1 &= ~WDTIFG;                        //
		while(!(IFG1 & WDTIFG)) {               // While WDT period..
			if(TACTL & TAIFG) {                 // Check for TimerA overflow
				freq += 0x10000L;               // Add 1 to msw of frequency
				TACTL &= ~TAIFG;                // Clear overflow flag
			}                                   //
		}                                       //
												//
		TACTL &= ~MC_2;                         // Stop counting - TimerA stop mode
		if(TACTL & TAIFG) freq += 0x10000L;     // Handle TimerA overflow that may have occured between
												//  last check of overflow and stopping TimerA
		freq |= TAR;                            // Merge TimerA count with overflow
		if(WDTCTL & WDTIS0) freq <<= 2;         // Multiply by 4 if using 250 ms gate
		set_gate(freq);                         // Adjust gate time if necessary
	}											//

		return freq;							//return measured value                                        //
}
