/* vim: set ts=4 sw=4: */
/* Filename    : mmap_ex1.c
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

int main(int argc, char *argv[])
{
	int		fd;
	int		sz_page;
	char	*p_map;
	char	a_tmp[4];

	if (argc != 2) {
		printf("Usage : %s <mmap file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	printf("System Page Size = %d\n", getpagesize());

	sz_page = 64;

	fd = open(argv[1], O_RDWR|O_CREAT, 0664);
	if (fd == -1) {
		printf("Fail: open(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	if (lseek(fd, sz_page, SEEK_SET) < 0) {
		printf("Fail: lseek(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Seek to %d. Then wirte %d byte\n", sz_page, sizeof(a_tmp));
	if (write(fd, a_tmp, sizeof(a_tmp)) < 0) {
		printf("Fail: write(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	printf("Open FD : %d, Page Size : %d\n", fd, sz_page);
	p_map = mmap((void *)0, 64, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (p_map == MAP_FAILED) {
		printf("Fail: mmap(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	sprintf(p_map, "%s", "Testing synchronized\n");

	/* synchronize 64 byte */
	printf(">> Memory Map Sync.\n");
	if (msync(p_map, 64, MS_SYNC) == -1) {
		printf("Fail: msync(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}

	printf(">> Memory Map Unmapped\n");
	if (munmap(p_map, 64) == -1) {
		printf("Fail: munmap(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	if (fsync(3) == -1) {
		printf("Fail: fsync(): (%d:%s)\n", errno, strerror(errno));
		return EXIT_FAILURE;
	}
	close(fd);

	return EXIT_SUCCESS;
}
