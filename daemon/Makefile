CC=gcc
LD=gcc

TARGETS=telemetryd telemetry-command
SRCS=telemetryd.c telemetry-command.c

CFLAGS=-Wall -O2
LDFLAGS=-lrt

default: $(TARGETS)

OBJS=$(SRCS:.c=.o)

$(OBJS): %.o:%.c
	@echo compiling $<
ifeq ($(V), 1)
	$(CC) $(CFLAGS) -c $< -o $@
else
	@$(CC) $(CFLAGS) -c $< -o $@
endif

$(TARGETS): %:%.o
	@echo linking into $@
ifeq ($(V), 1)
	$(LD) $< $(LDFLAGS) -o $@
else
	@$(LD) $< $(LDFLAGS) -o $@
endif

clean:
	rm -rf $(OBJS) $(TARGETS)
