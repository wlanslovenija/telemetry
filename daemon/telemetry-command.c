/* Author: Domen Puncer <domen@cba.si>.  License: WTFPL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <mqueue.h>


static void help(const char *prog)
{
	printf("usage: %s <command>\n", prog);
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		help(argv[0]);
		return -1;
	}

	mqd_t mq = mq_open("/telemetryd.command", O_WRONLY | O_NONBLOCK);
	if (mq == (mqd_t)-1) {
		perror("mq_open");
		return -1;
	}

	char msg[128];
	snprintf(msg, sizeof(msg), "command %s\n", argv[1]);

	int r = mq_send(mq, msg, strlen(msg), 0);
	if (r < 0) {
		perror("mq_send");
		return -1;
	}

	mq_close(mq);

	return 0;
}
