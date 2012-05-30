NAME=telemetry
SRCS=main.c gpio.c msp430_gpio.c circ_buf.c printf.c 1w.c bitbang_1w.c msp430_i2c.c
#MCU=msp430g2432
#MCU=msp430f2274 # msp430 ez2500
#MCU=msp430g2452 # launchpad mcu1
MCU=msp430g2553 # launchpad mcu2
CFLAGS=-mmcu=$(MCU) -g -Os -Wall -I. -DCONFIG_HZ=16000000UL
LDFLAGS=-mmcu=$(MCU) -g
CC=msp430-gcc

OBJS=$(SRCS:.c=.o)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

$(NAME).elf:$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	size $@

clean:
	rm -f $(OBJS) $(NAME).elf

cscope:
	cscope -Rb -s /usr/msp430/include/
