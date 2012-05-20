//******************************************************************************
//   main.c MSP430G2553 Frequency counter with UART output
//
//   Description: Measures DCO, ACLK and P1.0 frequency up to 16MHz
//   measurements are sent over UART at 9600 baud
//
//                MSP430G2553
//             -----------------
//         /|\|              XIN|-
//          | |                 |	32.768kHz
//          --|RST          XOUT|-
//            |                 |external clock input
//            |     P1.0/TA0CLK |<-----------
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   Musti
//   wlan slovenia http://dev.wlan-si.net
//   May 2012
//   Based on: 	TI, D.Dong MSP430G2xx3 examples
//   			Frequency Counter using Launchpad & Nokia 5110 LCD by oPossum
//******************************************************************************
#include  "USCI_A0_uart.h"
#include  "common.h"
#include  "frequencyCounter.h"

char buffer[16];
unsigned long freq=0;
char a=0;										//variable for foru loops

void cpu_init(void){
	  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	  DCOCTL = 0;                               // Run at 16 MHz
	  BCSCTL1 = CALBC1_16MHZ;                   //
	  DCOCTL  = CALDCO_16MHZ;                   //
	  BCSCTL2 = SELM_0;							// use DCO as system clock (MCLK)
	  BCSCTL1 |= DIVA_0;                        // ACLK/0
	  BCSCTL3 |= XCAP_3;						//12.5pF cap- setting for 32768Hz crystal
}

void main(void)
{
	cpu_init();
	uart_init();

	while(1){
		putsln("Measuring:");
		for(a=0;a<3;a++){							//rotate through all inputs
			freq=measureFrequency(a);				//0 - SMCLK, 1 - TACKL external, 2- ACLK internal
			ltos(freq,buffer);						// Convert to ASCII
			puts(buffer);                       	// Send result
			putsln("Hz");                       	// Send unit
		}
	}
}



