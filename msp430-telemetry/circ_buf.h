#ifndef _CIRC_BUF_H_
#define _CIRC_BUF_H_

#include "types.h"

/* A nice lockless circ buffer. One reader, one writer, or it will break */

struct circ_buf {
	u8 *data;
	unsigned size;
	volatile unsigned head;
	volatile unsigned tail;
};

void circ_buf_init(struct circ_buf *cb, u8 *buf, unsigned size);
unsigned circ_buf_len(struct circ_buf *cb);
unsigned circ_buf_free(struct circ_buf *cb);

/* returns number of characters succesfully written */
unsigned circ_buf_put(struct circ_buf *cb, const u8 *data, unsigned len);
/* returns number of characters succesfully read */
unsigned circ_buf_get(struct circ_buf *cb, u8 *data, unsigned len);

int circ_buf_put_one(struct circ_buf *cb, u8 data);
int circ_buf_get_one(struct circ_buf *cb);

#endif
