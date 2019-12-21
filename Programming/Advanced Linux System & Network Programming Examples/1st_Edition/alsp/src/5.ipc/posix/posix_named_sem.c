/* vim: set ts=4 sw=4: */
/* Filename    : posix_nameless_sem.c
 * Description : POSIX named semaphore
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#define NAME_POSIX_SEM	"/my_psem"
sem_t	*p_psem;	/* pointer for POSIX named semaphore */
int		i_semval;

int main()
{
	char	a_input[0xff];

	p_psem = sem_open(NAME_POSIX_SEM, O_CREAT|O_EXCL, 0600, 1);	/* create posix named sem */
	if (p_psem == SEM_FAILED) {
		if (errno != EEXIST) {
			fprintf(stderr, "FAIL: sem_open() (%s)\n", strerror(errno)); exit(EXIT_FAILURE);
		}
		p_psem = sem_open(NAME_POSIX_SEM, 0); /* attach to semaphore with specified name */
		printf("Attach to an existed sem\n");
	} else 
		printf("Create new sem\n");
	printf("sem = %p\n", p_psem);
	printf("[%ld] 'e' exit program, others entering crit. sect.\n", (long)getpid());
	while (1) {
		sem_getvalue(p_psem, &i_semval);
		printf("[%ld] waiting,  sem value = %d\n", (long)getpid(), i_semval);
		sem_wait(p_psem);	/* atomically decreased : P operation */
		printf("[%ld] On critical section.\n", (long)getpid());
		if (fgets(a_input, sizeof(a_input), stdin) == NULL) { /* error */ 
			exit(EXIT_FAILURE); 
		}
		if (a_input[0] == 'e') {
			sem_post(p_psem);	/* atomically increased : V operation */
			break; /* exit loop */
		}
		sem_post(p_psem);	/* atomically increased : V operation */
		sleep(1);
	}

	sem_close(p_psem);	/* deallocate sem memory */
	printf("[%ld] Would you like to remove named semaphore (y/n)", (long)getpid());
	if (fgets(a_input, sizeof(a_input), stdin) == NULL) { /* error */ exit(EXIT_FAILURE); }
	if (a_input[0] == 'y') {
		sem_unlink(NAME_POSIX_SEM);	/* remove semaphore from system */
	}

	return 0;
}
