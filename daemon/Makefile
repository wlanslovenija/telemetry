CC=gcc
LD=gcc

TARGET=telemetryd
SRCS=telemetryd.c

CFLAGS=-Wall -O2
LDFLAGS=-lrt

default: $(TARGET)

OBJS=$(SRCS:.c=.o)

%.o:%.c
	@echo compiling $<
ifeq ($(V), 1)
	$(CC) $(CFLAGS) -c $< -o $@
else
	@$(CC) $(CFLAGS) -c $< -o $@
endif

$(TARGET): $(OBJS)
	@echo linking into $(TARGET)
ifeq ($(V), 1)
	$(LD) $(LDFLAGS) $(OBJS) -o $@
else
	@$(LD) $(LDFLAGS) $(OBJS) -o $@
endif

clean:
	rm -rf $(OBJS) $(TARGET)
