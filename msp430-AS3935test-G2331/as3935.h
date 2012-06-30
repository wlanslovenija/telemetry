//******************************************************************************
//   as3935.h MSP430G2231 and AS3935 lightning detector test
//
//   Description: Library for using AS3935 lightning detector
//   i2c restart method is not required, memory is always read from register 0
//
//   Configuration of as3935 is done in this file, see datasheet for details.
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Developed in CCS v5
//******************************************************************************

#ifndef AS3935_H_
#define AS3935_H_

#include "common.h"
#include "console.h"
#include "i2c.h"
//addr device address in 8 bit format, lsb must be 0
//command is the register you specify
#define ADDR 0x00                   //chip i2c address
#define IRQ BIT4                    //interrupt input

#define DEBUG                       //uncomment for debug outputs

//configuration
#define NF_LEV 0x04                 //Threshold noise level, default 0x04 - highest possible
#define AFE_GB 0x1F                 //Receiver gain setting, 0x0E outdoor, 0x12
#define PWD 0x00                    //Power down 0-false, 1-true
#define WDTH 0x03                   //Watchdog threshold for going back to listen mode, 0x01 default, used for noise rejection
#define CL_STAT 0x01                //Reset lightning count with a sequence high-low-high
#define MIN_NUM_LIGH 0x00           //Minimum number of lightnings for IRQ, default 0x00 / 1 lightning
#define SREJ 0x02                   //Spike rejection threshold, default 0x02, increase for higher rejection
#define LCO_FDIV 0x00               //Programmable tuning frequency output to IRQ division factor, default 0x00 / 16
#define MASK_DIST 0x00              //Mask - turn off IRW for disturber event, default 0x00 - false, 0x01 -true
//test config
#define DISP_xCO 0x00               //Test output on IRQ pin, 0x01 for TRCO timer (expected 32.768kHz)
                                    //0x02 for SRCO system oscillator at about 1.1MHz
                                    //0.04 for LCO antenna resonant frequency output should be 500kHz +- 15kHz
#define TUN_CAP 0x08                //Tuning capacitor configuration 8pF steps

//uint8_t testConfig = (DISP_xCO<<5)|TUN_CAP;

//long to string converstion
uint8_t ltos(uint32_t val, uint8_t *str);
//delay function
void delay_ms(uint8_t ms);

//resets chip configuration to default
void reset_default(void);
//writes settings to the device
void write_settings(void);
//calibrates device clocks based on LCO
void calibrate_TRCO(void);
//clears lightning count statistics
void clear_STAT(void);
//determines the interrupt nature
void get_IRQ(void);
//sets the IRQ pin to display a clock
void set_xCO(uint8_t choice);
//creates a printout of sensor data and settings
void sensor_output(void);
//action when lightning is detected
void lightning_detected(void);

//write to the specified register
uint8_t i2c_pageWrite(uint8_t addr, uint8_t command, uint8_t count, uint8_t data[]);
//read a specified register
uint8_t i2c_byteRead(uint8_t addr, uint8_t command);
//read a page, starting from address and reading coun consecutive bytes
uint8_t i2c_pageRead(uint8_t addr, uint8_t command, uint8_t count, uint8_t data[]);
//write a single byte
uint8_t i2c_byteWrite(uint8_t addr, uint8_t command, uint8_t data);




#endif /* AS3935_H_ */
