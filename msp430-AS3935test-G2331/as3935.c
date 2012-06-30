//******************************************************************************
//   as3935.c MSP430G2231 and AS3935 lightning detector test
//
//   Description: Library for using AS3935 lightning detector
//   i2c restart method is not required, memory is always read from register 0
//
//   By: Musti musti@wlan-si.net
//   wlan slovenija http://dev.wlan-si.net
//   May 2012
//   Developed in CCS v5
//******************************************************************************
#include "as3935.h"

uint8_t cuitbuff[8];                            //number conversion buffer
uint8_t buffer[16];                             //stores the register readout from the AS3935
uint8_t storagevar;                             //general purpose storage
//assembling config into an arary
uint8_t config[9] = {(AFE_GB<<1)|(PWD),(NF_LEV<<4)|(WDTH),0x80|(CL_STAT<<6)|(MIN_NUM_LIGH<<4)|(SREJ),(LCO_FDIV<<6)|(MASK_DIST<<5),0x00,0x00,0x00,0x00,TUN_CAP};


/////////////////////////////////////////////////////////////////////////////////
//base 10 long to ASCII conversion method, works for unsigned long, can cast integers to it as well
uint8_t ltos(uint32_t val, uint8_t *str){
    uint32_t temploc = 0;
    uint32_t digit = 0;
    uint32_t strloc = 0;
    uint8_t tempstr[10];                            //32-bit number can be at most 10 ASCII digits;

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

void delay_ms(uint8_t ms){
    uint8_t i;
    for (i = 0; i<= ms; i++)
     __delay_cycles(500);                       //Built-in function that suspends the execution for 500 cicles
}

/////////////////////////////////////////////////////////////////////////////////


void reset_default(void){
    i2c_byteWrite(ADDR,0x3C,0x96);
}

void write_settings(void){
    i2c_pageWrite(ADDR,0x00,9,config);
}

//calibration method, must run after every power cycle
void calibrate_TRCO(void){
    P1IE &=~IRQ;                                        // IRQ interrupt disabled
    i2c_byteWrite(ADDR,0x3D,0x96);                      //send CALIB_RCO
    i2c_byteWrite(ADDR,0x00,config[0]|0x01);            //send power off
    delay_ms(2);                                        //2ms delay
    i2c_byteWrite(ADDR,0x00,config[0]&0xFE);            //send power on
    P1IFG &= ~(IRQ);                                    //IRQ IFG cleared
    P1IE |= IRQ;                                        // IRQ interrupt enabled
}
//clear lightning count
void clear_STAT(void){
    i2c_byteWrite(ADDR,0x02,config[2]|0x40);            //set CL_STAT
    i2c_byteWrite(ADDR,0x02,config[2]&~0x40);           //clear CL_STAT
    i2c_byteWrite(ADDR,0x00,config[3]|0x40);            //set CL_STAT
}

void get_IRQ(void){
    i2c_pageRead(ADDR, 0x08,9, buffer);                 //read the whole page
    switch(buffer[3]&0x0F){
        case 1:
            console_putsln("Noise");
            break;
        case 4:
            console_putsln("Dist");
            break;
        case 8:
            console_putsln("Lightning");
            lightning_detected();
            break;
        default:
            console_putsln("unknown");
            break;
    }
}

void set_xCO(uint8_t choice){
    i2c_byteWrite(ADDR,0x08,choice|TUN_CAP);
}


void lightning_detected(void){
    i2c_pageRead(ADDR, 0x08,9, buffer);
    //console_putsln("Lightning sensor AS3935 beta output.");
    console_puts("Lightning energy: ");
    ltos((uint32_t)(((buffer[6]&0x0F)<<16)&(buffer[5]<<8)&buffer[4]),cuitbuff); console_putsln(cuitbuff);
    console_puts("Distance: ");
    storagevar=(buffer[7]&0x3F);
    if(storagevar!=63){
        cuits(storagevar, cuitbuff);
        console_putsln(cuitbuff);
    }
    else{
        console_putsln("out of range");
    }
    console_newline();
    console_newline();

}
#ifdef DEBUG
void sensor_output(void){
    i2c_pageRead(ADDR, 0x08,9, buffer);
    //console_putsln("Lightning sensor AS3935 beta output.");
    console_puts("AFE_GB: ");
    cuits((buffer[0]>>1)&0x1F, cuitbuff); console_putsln(cuitbuff);
    console_puts("PWD: ");
    cuits( (0x01&buffer[0]), cuitbuff); console_putsln(cuitbuff);
    console_puts("NF_LEV: ");
    cuits( (buffer[1]>>4), cuitbuff);   console_putsln(cuitbuff);
    console_puts("WDTH: ");
    cuits( (buffer[1]&0x0F), cuitbuff); console_putsln(cuitbuff);
    lightning_detected();
    //console_puts("Raw: ");
    //console_puth(buffer,9);
    console_newline();
    console_newline();

}
#endif
//////////////////////////////////////////////////////////////////////////////////
//General R/W methods
uint8_t i2c_pageWrite(uint8_t addr, uint8_t command, uint8_t count, uint8_t data[])
{
    uint8_t c=0x00;
    char a;
    i2c_start();                    //S:  start
    c|=i2c_write8(addr|0x00);       //DW: write address 7-bit addr, LSB=0 for write
    c|=i2c_write8(command);         //WA: write command
    for(a=0;a<count;a++){
        c|=i2c_write8(data[a]);     //write data
    }
    i2c_stop();
    return c;
}
uint8_t i2c_byteWrite(uint8_t addr, uint8_t command, uint8_t data)
{
    uint8_t c=0x00;
    i2c_start();                    //S:  start
    c|=i2c_write8(addr|0x00);       //DW: write address 7-bit addr, LSB=0 for write
    c|=i2c_write8(command);         //WA: write command
    c|=i2c_write8(data);    //write data
    i2c_stop();
    return c;
}

uint8_t i2c_byteRead(uint8_t addr, uint8_t command){
    uint8_t c=0x00;
    uint8_t data=0x65;
    i2c_start();                    //S:  start
    c|=i2c_write8(addr|0x00); ;     //DW: write address 7-bit addr, LSB=0 for write
    c|=i2c_write8(command);         //WA: write command
    i2c_stop();
    i2c_start();                    //S:  start
    c|=i2c_write8(addr|0x01); ;     //DA: write address 7-bit addr, LSB=1 for read
    data=i2c_read8(NACK);           //receive data, send nack
    i2c_start();                    //S:  start
    i2c_stop();
    return data;
}
uint8_t i2c_pageRead(uint8_t addr, uint8_t command, uint8_t count, uint8_t data[]){
    uint8_t c=0x00;
    char a=0;                       //count variable
    //i2c_start();                  //S:  start
    //c|=i2c_write8(addr|0x00); ;   //DW: write address 7-bit addr, LSB=0 for write
    //c|=i2c_write8(command);       //WA: write command
    //i2c_stop();
    i2c_start();                    //S:  start
    c|=i2c_write8(addr|0x01); ;     //DA: write address 7-bit addr, LSB=1 for read
    for(a;a<count-1;a++){
        data[a]=i2c_read8(ACK);     //receive data, send ack
    }
    data[count-1]=i2c_read8(NACK);      //receive data, send ack
    i2c_stop();
    return c;
}
