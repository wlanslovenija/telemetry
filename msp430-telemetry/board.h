#ifndef _BOARD_H_
#define _BOARD_H_

#define F_CPU CONFIG_HZ
#include <in430.h>

#define udelay(x) (__delay_cycles((x)*(F_CPU/1000)/1000))
#if 0
static inline void udelay(const unsigned us)
{
	__delay_cycles(us*(F_CPU/1000000));
}
#endif

static inline void mdelay(int ms)
{
	while (ms--)
		__delay_cycles(F_CPU/1000);
}

#endif
