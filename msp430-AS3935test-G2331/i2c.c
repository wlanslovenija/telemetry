//******************************************************************************
//   i2c.c MSP430G2231 and AS3935 lightning detector test
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Based on:  TI, D.Dong MSP430G2xx3 examples
//   Developed in CCS v5
//******************************************************************************
#include "i2c.h"

void i2c_init(void)
{
    P1DIR |= SCL | SDA | LED; // Set SCL, SDA and LED as Output
    P1REN |= SCL | SDA; // Set Pull-Ups on SCL and SDA
    P1OUT &= ~LED; // Turn P1.0 Led off


    // enable SDA, SCL, SCLK, i2c mode, MSB, output enabled, hold in reset
    USICTL0 = USIPE7 | USIPE6 | USIMST | USIOE | USISWRST;

// USICTL0 Upper 8bit Register of 16bit USICTL Register
    // USIPE7   = P1.7 USI Mode, i2c SDA enabled
    // USIPE6   = P1.6 USI Mode, i2c SCL enabled
    // USIPE5   = P1.5 USI Mode, i2c Clock Input? (Not Set)
    // USILSB   = LSB Mode (Not Set = MSB)
    // USIMST   = Master Mode
    // USIGE    = Output Latch (Not Set = Clock Controlled)
    // USIOE    = Data Output Enable
    // USISWRST = USI Software Reset (Set to allow changing settings)


    // SMCLK / 128, and Reverse Clock Polarity
    USICKCTL = USIDIV_7 + USISSEL_2 + USICKPL;

// USICKCTL 8bit USI Clock Control Register
    // USIDIVx  = Clock Divider (Bit7-5, USIDIV_7 = Divide by 128)
    // USISSELx = Clock Source (For Master Mode, Bit4-2, USISSEL_2 = SMCLK)
    // USICKPL  = Clock Polarity (0 = Inactive Low, 1 = Inactive High)
    // USISWCLK = Software Clock State

    // I2C Mode
    USICTL1 = USII2C;

// USICTL1 Lower 8bit Register of 16bit USICTL Register
    // USICKPH   = Clock Phase (0 = Data Changed, then Captured, 1 = Data Captured, then Changed)
    // USII2C    = I2C mode
    // USISTTIE  = START condition Interrupt
    // USIIE     = USI Counter Interrupt
    // USIAL     = Arbitration Lost Notification
    // USISTP    = STOP condition Notification
    // USISTTIFG = START condition Int. Flag
    // USIIFG    = USI Counter Int. Flag

    // release from reset
    USICTL0 &= ~USISWRST;
}

void i2c_start(void)
{
    P1OUT |= LED;  // Turn P1.0 Led on

// Send i2c START condition
    USISRL = 0x00; // Load USISRL Lower Byte Shift Register MSB with 0 for i2c START
    USICTL0 |= USIGE | USIOE; // Force Output Latch, And Enable Data Output Bit (High to Low SDA while SCL is High)
    USICTL0 &= ~USIGE; // Clear Output Latch (Return to Clock Control)
}

void i2c_restart(void)
{
    USISRL = 0xFF;                // this will prevent a stop cond
    USICTL0 |= USIOE;             // SDA = output
    USICNT |=  0x01;              // set count=1 to trigger next state
    USISRL = 0x0FF;               // USISRL = 1 to release SDA
    USICTL0 |= USIGE;             // Transparent latch enabled
    USICTL0 &= ~(USIGE+USIOE);    // Latch/SDA output disabled

// Send i2c reSTART condition
//    USICTL0 |= USIGE | USIOE; // Force Output Latch, And Enable Data Output Bit (High to Low SDA while SCL is High)
//    USISRL = 0xFF; //Load MSB of USISRL with
//    USICNT |= 0x01;//set USICNT to 1 to generate a surplus clock cycle
    //USICTL0 &= ~USIGE; // Clear Output Latch (Return to Clock Control)
}


void i2c_stop(void)
{
// Prepare i2c STOP condition
    USICTL0 |= USIOE; // Enable Data Output Bit (Turn SDA into Output)
    USISRL = 0x00; // Load USISRL Lower Byte Shift Register MSB with 0 for i2c STOP
    USICNT = 1; // Load USICNT Counter with number of Bits to Send. USIIFG Auto-Cleared
    // Data TXed by USI I2C
    while((USICTL1 & USIIFG) != 0x01); // Delay, Wait for USIIFG, Counter down to 0

// Send i2c STOP condition
    USISRL = 0xFF; // Load USISRL Lower Byte Shift Register MSB with 1 for i2c STOP
    USICTL0 |= USIGE; // Force Output Latch (Low to High SDA while SCL is High)
    USICTL0 &= ~USIOE & ~USIGE ; // Clear Data Output Enable Bit and Output Latch (Release SCL)

    P1OUT &= ~LED; // Turn P1.0 Led off
}


uint8_t i2c_write8(uint8_t c)
{
// TX
    USICTL0 |= USIOE; // Enable Data Output Bit (Turn SDA into Output)
    USISRL = c; // Load USISRL Lower Byte Shift Register with 8 Bit data (Byte)
    USICNT = 8; // Load USICNT Counter with number of Bits to Send. USIIFG Auto-Cleared
    // Data TXed by USI I2C
    while((USICTL1 & USIIFG) != 0x01); // Delay, Wait for USIIFG, Counter down to 0

// RX
    // Data TXed. Ready to Receive (n)ACK from i2c Slave
    USICTL0 &= ~USIOE; // Clear Data Output Enable Bit (Turn SDA into Input)
    USICNT = 1; // Load USICNT Counter with number of Bits to Receive. USIIFG Auto-Cleared
    // Data RXed by USI I2C
    while((USICTL1 & USIIFG) != 0x01); // Delay, Wait for USIIFG, Counter down to 0

// Return Data
    c = USISRL; // LSB of USISRL Holds Ack Status of 0 = ACK (0x00) or 1 = NACK (0x01)
    return c;
}

uint8_t i2c_read8(uint8_t acknack)
{
// RX
    USICTL0 &= ~USIOE; // Clear Data Output Enable Bit (Turn SDA into Input)
    USISRL = 0x00; // Clear USISRL Lower Byte Shift Register (Byte)
    USICNT = 8; // Load USICNT Counter with number of Bits to Receive. USIIFG Auto-Cleared
    // Data RXed by USI I2C
    while((USICTL1 & USIIFG) != 0x01); // Delay, Wait for USIIFG, Counter down to 0

// Copy Data to c
    uint8_t c;
    c = USISRL; // USISRL Holds Received Data

// TX
    // Data RXed. Ready to Send (n)ACK to i2c Slave
    USICTL0 |= USIOE; // Enable Data Output Bit (Turn SDA into Output)
    USISRL = acknack; // Load USISRL Lower Byte Shift Register MSB with acknack (0x00 = Ack, 0xFF = Nack)
    USICNT = 1; // Load USICNT Counter with number of Bits to Send. USIIFG Auto-Cleared
    // Data TXed by USI I2C
    while((USICTL1 & USIIFG) != 0x01); // Delay, Wait for USIIFG, Counter down to 0

// Return Data
    return c;
}
