
TARGETS=telemetryd telemetry-command
SRCS=telemetryd.c telemetry-command.c

default: $(TARGETS)

OBJS=$(SRCS:.c=.o)

$(OBJS): %.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGETS): %:%.o
	$(CC) $(LDFLAGS) -o $@ $< -lrt

clean:
	rm -rf $(OBJS) $(TARGETS)
