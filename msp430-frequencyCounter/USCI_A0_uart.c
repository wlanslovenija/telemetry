//******************************************************************************
//   USCI_A0_uart.c MSP430G2553 simple UART output
//
//   Description: Basic UART communication with 16MHz clock.
//	 Additional formating functions for easier use. RX has 16 char buffer
//
//   Musti
//   wlan slovenia http://dev.wlan-si.net
//   May 2012
//   Based on: 	TI, D.Dong MSP430G2xx3 examples
//******************************************************************************
#include  "USCI_A0_uart.h"

bool hasReceived;		// Lets the program know when a byte is received
uint8_t rxbuffer[16];		//serial buffer, simple linear until break character /r
uint8_t bitTrack;		//serial buffer counter
uint8_t count;			//serial buffer counter
uint8_t RXbyte;			//serial buffer counter


void uart_init(void)
{
	  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	  P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
	  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	  UCA0BR0 = 0x86;                            // 16MHz 9600
	  UCA0BR1 = 0x06;                            // 16MHz 9600
	  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
	  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

	  __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled
}

void puts(char *str)
{
    while(*str!=0)
       putc(*str++);
}
void putsln(char *str)
{
    while(*str!=0)
       putc(*str++);
    putc('\r');
    putc('\n');

}

void putc(uint8_t data){
	while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
	UCA0TXBUF = data;
	hasReceived=FALSE;
}

void newline(void)
{
    putc('\r');
    putc('\n');
}

bool gets(uint8_t data[]){
    if (!hasReceived)
        return FALSE;
    for(count=0;count<16;count++){
    	data[count] = rxbuffer[count];
    }
    hasReceived = FALSE;
    return TRUE;
}

//RX interrupt
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	RXbyte=UCA0RXBUF;
	putc(RXbyte); 								//loopback
	if(RXbyte=='\r'){
		 hasReceived = TRUE;}
	else{
		if(bitTrack>=16){
			rxbuffer[bitTrack]=RXbyte;
			bitTrack=0;
		}
		else{
			rxbuffer[bitTrack++]=RXbyte;
		}
	}
}
