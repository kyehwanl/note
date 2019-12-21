/* vim: set ts=4 sw=4: */
/* Filename    : posix_shm.c
 * Description : shared memory test code for POSIX shm
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#define _POSIX_C_SOURCE		200112L
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define NAME_POSIX_SHM		"/proj_mytest"
#define SZ_SHM_SEGMENT		1024

int shm_fd;
char *shm_ptr;

int main()
{
	char	a_input[0xff];

	printf("* SHM Name : %s\n", NAME_POSIX_SHM);
	if ((shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR|O_CREAT|O_EXCL, 0660)) > 0) {
		printf("* Create SHM\n");
		if (ftruncate(shm_fd, SZ_SHM_SEGMENT) == -1) exit(EXIT_FAILURE);
	} else {
		if (errno != EEXIST) { /* already exist shm */
			printf("FAIL: shm_open(): %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR, 0);
		if (shm_fd == -1) {
			printf("FAIL: shm_open(): %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* make mmap on shm */
	shm_ptr = (char *)mmap(NULL, SZ_SHM_SEGMENT, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) {
		printf("FAIL: mmap()\n");
		exit(EXIT_FAILURE);
	}

	printf("'*' PRint current shm.\n");
	printf("'.' Exit.\n");
	printf("otherwise change shm to your input.\n");
	while (1) {
		printf("\n>>");
		if (fgets(a_input, sizeof(a_input), stdin) == NULL) {
			/* error */
		}
		if (a_input[0] == '.') {
			break;
		} else if (a_input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);
		} else {
			memcpy(shm_ptr, a_input, strlen(a_input));
		}
	}
	munmap(shm_ptr, SZ_SHM_SEGMENT);

	printf("* Would you remove shm (name : %s) (y/n)", NAME_POSIX_SHM);
	fgets(a_input, sizeof(a_input), stdin);
	if (a_input[0] == 'y') {
		shm_unlink(NAME_POSIX_SHM);	/* remove shm */
	}

	return 0;
}
