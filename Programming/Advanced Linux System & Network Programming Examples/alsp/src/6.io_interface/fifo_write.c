/* vim: set ts=4 sw=4: */
/* Filename    : fifo_write.c
 * Description : test writing to FIFO(named PIPE)
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define PATH_FIFO	"/tmp/my_fifo"

int main()
{
	int		fd, n_write;
	char	a_input[0xff];

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

