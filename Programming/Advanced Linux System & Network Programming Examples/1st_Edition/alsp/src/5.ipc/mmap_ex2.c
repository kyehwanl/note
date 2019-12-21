/* vim: set ts=4 sw=4: */
/* Filename    : mmap_ex2.c
 * Description : Memory mapped file
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#define MMAP_FILENAME	"shared_mmap"
#define MMAP_SIZE		64

char	contents[] = "(*) What's differents between Shared mmap and Private mmap?      - end - ";

int main(int argc, char *argv[])
{
	int		fd;
	int		flag_mmap;
	char	*p_map;
	char	a_input[100];
	int		n_write;

	if (argc != 2) {
		printf("Usage : %s <s | p>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* make a file has 68 byte */
	fd = open(MMAP_FILENAME, O_RDWR|O_CREAT, 0664);
	if (fd == -1) {
		printf("Fail: open(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	if ((n_write = write(fd, contents, sizeof(contents))) < 0) {
		printf("Fail: lseek(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("%d byte writing... to file -> %s\n", n_write, MMAP_FILENAME);

	/* create mmap */
	if (*argv[1] == 's') {
		flag_mmap = MAP_SHARED;
	} else {
		flag_mmap = MAP_PRIVATE;
	}

	p_map = mmap((void *)0, MMAP_SIZE, PROT_READ|PROT_WRITE, flag_mmap, fd, 0);
	if (p_map == MAP_FAILED) {
		printf("Fail: mmap(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	while (1) {
		printf("'*' print Current mmap otherwise input text to mmap. >>");
		if (fgets(a_input, sizeof(a_input), stdin) == NULL) {
			/* error */
		}
		if (a_input[0] == '*') {
			printf("Current mmap -> '%.*s'\n", MMAP_SIZE, p_map);
		} else {
			memcpy(p_map, a_input, strlen(a_input));
			if (msync(p_map, MMAP_SIZE, MS_SYNC) == -1) { /* synchronize mmap and file = flush */
				printf("Fail: msync(): (%d:%s)\n", errno, strerror(errno));
				return EXIT_FAILURE;
			}
		}
	}

	return EXIT_SUCCESS;
}
