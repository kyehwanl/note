/* vim: set ts=4 sw=4: */
/* Filename    : pthread_mutex.c
 * Description : shared mutex with other processes
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#ifdef _XOPEN_SOURCE
# if (_XOPEN_SOURCE - 0) < 500
# undef _XOPEN_SOURCE
# endif
#endif
#define _XOPEN_SOURCE 500
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#ifdef USE_PSHARED
#include "sysv_shm.h"
#define MY_SHM_KEY		0x123000
int		shm_id;				/* shared memory id */
void 	*shm_ptr;			/* shared memory pointer */
int		pshared = PTHREAD_PROCESS_SHARED;
#endif
#define MAX_THREAD	3

struct thread_arg {
	pthread_t	pt_id;	/* thread id 	*/
	int			idx;	/* thread index */
} thr_arg[MAX_THREAD];

struct share_data {
	int			i_data;	/* shared data for mutex example */
	pthread_mutex_t	mutex_data;
	pthread_mutexattr_t	mutexattr_data;
} *p_shr_data;

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);

int main(int argc, char ** argv)
{
	int		i; 
	struct timeval tv;

	if (argc != 2) {
		printf("Usage: %s <c|a>\nc:create shm, a:attach shm\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* init var, random seed */
	memset(thr_arg, 0x00, sizeof(struct thread_arg) * MAX_THREAD);
	gettimeofday(&tv, NULL);
	srandom((unsigned int)tv.tv_usec);

	/* get shr memory */
	shm_id = sysv_shmget(NULL, MY_SHM_KEY, sizeof(struct share_data), 0664);
	if (shm_id < 0) {
		printf("[MAIN] FAIL: sysv_shmget()\n"); exit(EXIT_FAILURE);
	}
	shm_ptr = (char *) shmat(shm_id, 0, 0);  /* attach shm */
	if (shm_ptr == NULL) {
		printf("[MAIN] FAIL: shmat()\n"); exit(EXIT_FAILURE);
	}
	p_shr_data = (struct share_data *) shm_ptr;	/* shared data in SHM */

	if (argv[1][0] == 'c') {
		memset(p_shr_data, 0x00, sizeof(struct share_data));
		pthread_mutexattr_init(&p_shr_data->mutexattr_data);
		pthread_mutexattr_settype(&p_shr_data->mutexattr_data, PTHREAD_MUTEX_ERRORCHECK);
		pthread_mutexattr_setpshared(&p_shr_data->mutexattr_data, pshared);
		pthread_mutex_init(&p_shr_data->mutex_data,(const pthread_mutexattr_t *) &p_shr_data->mutexattr_data);
	}

	/* Start thread */
	printf("* Create thread\n\t-- press any key --"); getchar();
	for(i=0; i<MAX_THREAD; i++) {
		thr_arg[i].idx = i;
		sleep(2);
		if (pthread_create(&thr_arg[i].pt_id, NULL, start_thread, (void *)&thr_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create()\n");
			return 0;
		}
		printf("[MAIN] Create thread : tid = %lu\n", thr_arg[i].pt_id);
	}

	printf("[MAIN] Will join All threads!\n");
	clean_thread(thr_arg);

	if (argv[1][0] == 'c') {
		printf("* remove shm\n\t-- press any key --"); getchar();
		sysv_shmrm(shm_id);  /* remove shm */
	}
	return 0;
}

/* Name : start_thread
 * Desc : thread function with message
 * Argv : int idx (child index)
 * Ret  : None
 */
void *start_thread(void *arg)
{
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	struct timeval tv;
	int		i, ret;

	if ((ret = pthread_mutex_lock(&p_shr_data->mutex_data))) {
		if (ret == EDEADLK)
			printf("\t EDEADLK detected\n");
		else
			printf("\t errno : %d\n", ret);
	}
	printf("\t[PID:%d, Thread (No:%d) (ID:%lu)]\n", getpid(), t_arg->idx, t_arg->pt_id);
	printf("\t\twill sleep %d sec.\n", i = rand_r((unsigned int *)&tv.tv_usec) % 7);
	sleep(i);
	p_shr_data->i_data = i;	/* update data */
	if ((ret = pthread_mutex_unlock(&p_shr_data->mutex_data))) {
		printf("Error code: %d\n", ret);
	}
	return t_arg; /* alternative func, pthread_exit() */
}

/* Name : clean_thread
 * Desc : clean(join) thread
 * Argv : None
 * Ret  : None
 */
void clean_thread(struct thread_arg *t_arg)
{
	int			i;
	struct thread_arg *t_arg_ret;

	for (i=0; i<MAX_THREAD; i++, t_arg++) {
		pthread_join(t_arg->pt_id, (void **)&t_arg_ret);
		printf("[Join] Thread (No:%d) (ID:%lu)]\n", t_arg->idx, t_arg->pt_id);
	}
	printf("[Clean] All thread are joined.\n");
}
