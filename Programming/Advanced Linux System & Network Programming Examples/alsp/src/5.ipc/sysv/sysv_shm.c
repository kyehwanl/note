/* vim: set ts=4 sw=4: */
/* Filename    : sysv_shm_info.c
 * Description : IPC_INFO
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include "sysv_shm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define SZ_SHM_SEGMENT		1024

int	shm_id;
char *shm_ptr;

int main(int argc, char *argv[])
{
	char	a_input[0xff];

	printf("c : Create shared memory with IPC creation.\n");
	printf("number : attach shared memory with IPC id number.\n>>");
	fgets(a_input, sizeof(a_input), stdin);
	if (a_input[0] == 'c') {
		shm_id = sysv_shmget(NULL, IPC_PRIVATE, sizeof(char) * SZ_SHM_SEGMENT, 0664); /* Create shm */
		if (shm_id == -1) {
			fprintf(stderr, "FAIL: sysv_shmget()\n");
			exit(EXIT_FAILURE);
		}
	} else {
		shm_id = atoi(a_input);
	}
	printf("* SHM IPC id : %d\n", shm_id);

	if ((shm_ptr = (char *)shmat(shm_id, 0, 0)) == NULL) {
		fprintf(stderr, "FAIL: shmat()\n"); 
		exit(EXIT_FAILURE);
	}
	printf("'*' Print current shm.\n'.' Exit. Otherwise input text to shm.\n");
	printf("'?' print shm info\n");
	while (1) {
		printf("\n>>");
		if (fgets(a_input, sizeof(a_input), stdin) == NULL) {
			/* error */
		}
		if (a_input[0] == '.') {
			break;
		} else if (a_input[0] == '?') {
			struct shmid_ds shmds_buf;
			if (shmctl(shm_id, IPC_STAT, &shmds_buf) == -1) {
				printf("FAIL: shmctl()\n");
			} else {
				printf("SHM: size(%d) # of attach(%ld)\n", 
						shmds_buf.shm_segsz, shmds_buf.shm_nattch);
			}
		} else if (a_input[0] == '*') {
			printf("shm -> '%.*s'\n", SZ_SHM_SEGMENT, shm_ptr);
		} else {
			memcpy(shm_ptr, a_input, strlen(a_input));
		}
	}

	printf("* Would you remove shm (IPC id : %d) (y/n)", shm_id);
	fgets(a_input, sizeof(a_input), stdin);
	if (a_input[0] == 'y') {
		sysv_shmrm(shm_id);	/* remove shm */
	}

	return 0;
}
