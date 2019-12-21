/* vim: set ts=4 sw=4: */
/* Filename    : sysv_sem.c
 * Description : Critical section testing, so use specify one kind of IPC, SysV semamphore.
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : Critical section means only one process attaching/processing at same time.
 */
#include "sysv_sem.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#define	USAGES	"%s [# of child process]\n"

int handle_criticalsect(int, int);

int main(int argc, char *argv[])
{
	int	i, n_child, status, sem_id;

	if (argc != 2) {
		printf(USAGES, argv[0]); exit(EXIT_FAILURE);
	}
	n_child = atoi(argv[1]);
	sem_id = sysv_semget(NULL, 0x12340001, 1, 1, 0660); /* get SysV sem */
	for(i=0; i<n_child; i++) {
		switch(fork()) {
			case 0:		/* child */
				handle_criticalsect(sem_id, i);
				exit(EXIT_SUCCESS);
			case -1: 	/* error */
				fprintf(stderr, "FAIL: fork() [%s:%d]\n", __FUNCTION__, __LINE__);
				exit(EXIT_FAILURE);
			default:	/* parent */
				break;
		}
	}
	for(i=0; i<n_child; i++) {
		waitpid(-1, &status, 0);
	}
	if (sysv_semrm(sem_id) == -1) { /* remove sem from system */
		perror("FAIL: sysv_semrm"); return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int handle_criticalsect(int sem_id, int idx_child)
{
	printf("[Child:%d] Waiting.. semval(%d), semncnt(%d), semzcnt(%d)\n", 
			idx_child, semctl(sem_id, 0, GETVAL), semctl(sem_id, 0, GETNCNT), semctl(sem_id, 0, GETZCNT));
	sysv_semwait(sem_id, 0);	/* atomically decreased */
	printf("\t[Child:%d] On critcal section\n", idx_child);
	if (idx_child == 2) abort();
	sleep(2);
	sysv_sempost(sem_id, 0); 	/* atomically increased */
	printf("\t[Child:%d] Exiting\n\n", idx_child);
	return 0;
}
