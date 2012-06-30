//******************************************************************************
//   console.c MSP430G2231 and AS3935 lightning detector test
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
#include "common.h"
#include "console.h"
#include "serial.h"

void console_putc(uint8_t c)
{
    putc(c);
}

void console_newline(void)
{
    console_putc('\r');
    console_putc('\n');
}

void console_puts(const uint8_t *str)
{
    while(*str!=0)
        console_putc(*str++);
}
void console_putsln(const uint8_t *str)
{
    while(*str!=0)
        console_putc(*str++);
    console_newline();
}

//prints a hex value of a string
void console_puth(const uint8_t *str,uint8_t count)
{
    uint8_t a;
    for(a=0;a<count;a++){
        console_puts(" 0x");
            if(((*str>>4)&0x0F)<=9)                     //check if the upper nibble is 0-9
                console_putc(((*str>>4)&0x0F)+48);      //hex 0x30 is ASCII 0
            else
                console_putc(((*str>>4)&0x0F)+55);      //hex 0x41 is ASCII 0
            if((*str&0x0F)<=9)                          //check if the upper nibble is 0-9
                console_putc((*str&0x0F)+48);           //hex 0x30 is ASCII 0
            else
                console_putc((*str&0x0F)+55);           //hex 0x41 is ASCII 0
        str++;                                          //increment pointer
    }
}


void cuits( uint16_t value, uint8_t* buffer )
{ 
    uint8_t p=0;
    uint16_t s =0;

    if ( value == 0 ){
        buffer[0]='0';
        buffer[1]='\0';
        return;
    }
    else if ( value < 0 ){
        buffer[p++] = '-';
        value = (value<0)?-value:value;
    }
    for (s=10000; s>0; s/=10 ){
        if ( value >= s ){
            buffer[p++] = (uint8_t)(value/s) + '0';
            value %= s;
        }
    }
    buffer[p] = '\0';
}

