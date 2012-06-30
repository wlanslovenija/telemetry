//******************************************************************************
//   main.c MSP430G2231 and AS3935 lightning detector test
//
//   Description: Communicated with AS3935 via I2C, calibrates it, handles
//   lightning detection events, transmits detected lightning distance and power
//   Automatic detector tuning not yet implemented.
//   Data sent over UART at 9600 baud
//
//                MSP430G2231
//             -----------------
//         /|\|              XIN|-
//          --|RST          XOUT|-
//            |     P1.3/Button |<-----------toggle modes
//            |                 |
//            |     P1.1/UCA0RXD|<------------9600 - 8N1
//            |     P1.2/UCA0TXD|------------>
//            |     P1.4/IRQ    |<------------interrupt
//            |     P1.6/SCL    |------------> I2C scl
//            |     P1.7/SDA    |<------------ I2C sda
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Based on:  MSP430G2*** examples,
//              Software async serial tx/rx without timer by oPossum on 43oh.com
//   Developed in CCS v5
//******************************************************************************
#include "common.h"             //common includes
#include "console.h"            //serial functions
#include "as3935.h"             //lightning detector

uint8_t irqstat;                //monitoring varaible
uint8_t button=0;               //button variable

void cpu_init(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WatchDog Timer

    BCSCTL1 = CALBC1_1MHZ;      // Set range to 1mhz Calibrated Range
    DCOCTL = CALDCO_1MHZ;       // Set DCO to 1mhz Calibrated DCO
                                // SMCLK = DCO = 1MHz
    P1DIR &= ~(IRQ|BIT3);       // Push Port 1 as input
    P1IE |= BIT3;               // P1.3 interrupt enabled
    P1IES &= ~IRQ;              // low to high for IRQ
    P1IFG &= ~(IRQ|BIT3);       // P1.3 and IRQ IFG cleared

    //MUST not be ENABLED, GIE and sleep mode overrides it and disables interrupts
    //__enable_interrupt();         // enable all interrupts
}


#ifdef DEBUG                    //only if enabled in as3935.h
//debug only function
void switch_xCO(){
                                //requires global variable button
    switch(button) {
        default:
            button=0;
        case 0:
            set_xCO(0x00);      //Do not display any clock on IRQ pin
            P1IE |= IRQ;        // IRQ interrupt enabled
            break;
        case 1:
            set_xCO(0x80);
            console_putsln("LCO");//display LCO - antenna freq divided by 16
            P1IE &=~IRQ;        // IRQ interrupt disabled
            break;
        case 2:
            set_xCO(0x40);
            console_putsln("SRCO");//display SRCO about 1MHz
            P1IE &=~IRQ;        // IRQ interrupt disabled
            break;
        case 3:
            set_xCO(0x20);
            console_putsln("TRCO");//display TRCO, about 32.768kHz
            P1IE &=~IRQ;        // IRQ interrupt disabled
            break;
    }
}
#endif

void main(void)
{
    cpu_init();                 //initialize
    i2c_init();                 //initialize i2c
    serial_setup(BIT1, BIT2, 1000000 / 9600);//initialize serial port
    delay_ms(200);              //delay for everything to stabilise
    console_putsln("Initialized.");//confirm initialization
    sensor_output();            //display the data from the sensor
    reset_default();
    write_settings();           //configure sensor
    calibrate_TRCO();           //calibrate clocks, enable IRQ

    while(1){                   // enableinterrupts/enterlow-powermode3
        __bis_SR_register(LPM3_bits + GIE);//waiting or interrupts
    }
}

// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    #ifdef DEBUG
    //change clock output on IRQ pin
    if((P1IFG&BIT3)!=0){        //button interrupt
        P1IFG &= ~(BIT3);       //button IFG cleared
        button++;               //increment selection
        switch_xCO();           //set the clock output, debug only option
        sensor_output();        //printout
        return;

    }
    #endif
    //output interrupt nature if not in the clock output mode
    if((P1IFG&IRQ)!=0){         //IRQ interrupt
        P1IFG &= ~(IRQ);        //IRQ IFG cleared
        delay_ms(4);            //minimum settling time
        get_IRQ();              //get IRQ nature
    }
}
