//******************************************************************************
//   serial.h MSP430G2553 Frequency counter with UART output
//   Source: Software async serial tx/rx without timer by oPossum on 43oh.com
//   Developed in CCS v5
//******************************************************************************
void serial_setup (unsigned out_mask, unsigned in_mask, unsigned duration);
void putc (unsigned);
void puts (char *);
unsigned getc (void);
