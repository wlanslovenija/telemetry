#ifndef _1W_H_
#define _1W_H_

#include "types.h"

typedef struct {
	u8 bytes[8];
} w1_addr_t;

/* functions the master must implement */

struct w1_master {
	void *priv;
	int (*probe)(struct w1_master *master);
	int (*w1_reset)(struct w1_master *master);
	int (*w1_triplet)(struct w1_master *master, int dir);
	int (*w1_write)(struct w1_master *master, u8 data);
	int (*w1_read)(struct w1_master *master);
};


/**
 * quick overview of 1-wire commands
 * READ_ROM - reads the 8-byte device signature (only one device on bus!)
 * SKIP_ROM - selects only one device on bus
 * MATCH_ROM - selects the device by signature (multiple devices on bus)
 * SEARCH_ROM - enumerates the devices on a bus.
 */

/* public API */

/* reads rom (family, addr, crc) from one device, in case of multiple this command is an error */
int w1_read_rom(struct w1_master *w1, w1_addr_t *addr);

/* selects one device, and puts it into transport layer mode */
int w1_match_rom(struct w1_master *w1, w1_addr_t addr);

/* in case there's only one device on bus, skip rom is used to put it into transport layer mode */
int w1_skip_rom(struct w1_master *w1);

int w1_scan(struct w1_master *w1, w1_addr_t addrs[], int num);

/* for transport layer */
int w1_write(struct w1_master *w1, u8 data);
int w1_read(struct w1_master *w1);

u8 crc8r(u8 *p, int len);

#endif
