/* vim: set ts=4 sw=4: */
/* Filename    : posix_nameless_sem.c
 * Description : POSIX nameless semaphore
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#define _POSIX_C_SOURCE		200112L
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define NAME_POSIX_SHM	"/my_pshm4sem"
#define	USAGES	"%s [# of child process]\n"

int handle_criticalsect(int);
int		shm_fd;
sem_t	*p_psem;	/* POSIX semaphore */

int main(int argc, char *argv[])
{
	int	i, n_child, status;
	int		pshared;	/* process shared flag, means 0:not shared, 1:shared */

	if (argc != 2) {
		printf(USAGES, argv[0]);
		exit(EXIT_FAILURE);
	}

	/* share semaphore to the other process, semaphore object should be on shared memory. */
	printf("* SHM Name : %s\n", NAME_POSIX_SHM);
	if ((shm_fd = shm_open(NAME_POSIX_SHM, O_RDWR|O_CREAT|O_EXCL, 0660)) > 0) {
		printf("* Create SHM\n");
		if (ftruncate(shm_fd, sizeof(sem_t)) == -1) exit(EXIT_FAILURE);
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

	/* allocate semaphore object address on shm */
	p_psem = (sem_t *)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (p_psem == MAP_FAILED) {
		printf("FAIL: mmap()\n");
		exit(EXIT_FAILURE);
	}


	n_child = atoi(argv[1]);
	pshared = 1;	/* shared semaphore with child process */

	sem_init(p_psem, pshared, 1);	/* init posix sem */
	for(i=0; i<n_child; i++) {
		switch(fork()) {
			case 0:		/* child */
				handle_criticalsect(i);
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
	printf("[Parent] Stop all child processes and remove nameless semaphore.\n");
	sem_destroy(p_psem);

	return 0;
}

int handle_criticalsect(int idx_child)
{
	int		sem_value;

	sem_getvalue(p_psem, &sem_value);
	printf("[Child %d] Waiting for entering:sem_val(%d)\n", idx_child, sem_value);
	sem_wait(p_psem);	/* atomically decreased : P operation */
	printf("  ++ <Child %d> On section\n", idx_child);

	if (idx_child == 2) abort(); sleep(2);

	sem_post(p_psem);	/* atomically increased : V operation */
	printf("  ++ <Child %d> Exiting\n\n", idx_child);
	return 0;
}
