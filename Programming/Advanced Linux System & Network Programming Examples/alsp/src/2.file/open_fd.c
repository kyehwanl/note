/* vim: set ts=4 sw=4: */
/* Filename    : open_fd.c
 * Description : open with file descriptor
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>		/* EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILENAME	"fd_test.log"
#define ERR_MSG		"ERR: Check your program\n" 
#define TEST_MSG	"TEST: Low level file handle with file descriptor\n"

int main()
{
	int		fd;

	printf("MSG SIZE:%d\n", sizeof(ERR_MSG));
	fd = open(FILENAME, O_CREAT|O_RDWR, 0644);
	if (fd == -1) {
		write(STDERR_FILENO, ERR_MSG, sizeof(ERR_MSG));
		exit(EXIT_FAILURE);
	}

	printf("opened fd = %d", fd);		/* buffering */
	write(fd, TEST_MSG, sizeof(TEST_MSG)); 

	close(fd);
	return (EXIT_SUCCESS);
}

