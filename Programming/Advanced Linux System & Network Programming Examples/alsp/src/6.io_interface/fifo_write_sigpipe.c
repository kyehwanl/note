/* vim: set ts=4 sw=4: */
/* Filename    : fifo_write_sigpipe.c
 * Description : test writing to FIFO(named PIPE)
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : install signal handler on SIGPIPE
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define PATH_FIFO	"/tmp/my_fifo"
int inst_sighandler();
void sighandler_pipe(int);

int main()
{
	int		fd, n_write;
	char	a_input[0xff];

	if (inst_sighandler()) {
		/* error */
	}

	if ((fd = open(PATH_FIFO, O_WRONLY, 0644)) == -1) {
		fprintf(stderr, "FAIL: open() (%d:%s)\n", errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (1) {
		printf("Plz write message to FIFO >>");
		if (fgets(a_input, sizeof(a_input), stdin) == NULL) {
			/* error */
		}
		
		if ((n_write = write(fd, a_input, strlen(a_input))) == -1) { /* error */ }
		printf("* Writing %d bytes...\n", n_write);
	}
	return (EXIT_SUCCESS);
}

int inst_sighandler()
{
	struct sigaction sa_pipe;

	sa_pipe.sa_handler = sighandler_pipe;
	sigfillset(&sa_pipe.sa_mask);
	sa_pipe.sa_flags = SA_RESTART;

	if (sigaction(SIGPIPE, &sa_pipe, NULL) == -1) {
		/* error */
	}

	return 0;
}

void sighandler_pipe(int signum)
{
	printf(">SIGPIPE(%d) occured\n", signum);
}
