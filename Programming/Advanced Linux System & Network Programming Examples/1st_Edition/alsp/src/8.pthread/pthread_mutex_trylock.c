/* vim: set ts=4 sw=4: */
/* Filename    : pthread_mutex_trylock.c
 * Description : 
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#ifndef __USE_GNU
#define __USE_GNU
#endif
#ifndef __USE_UNIX98
#define __USE_UNIX98
#endif
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#define MAX_THREAD	3

struct thread_arg {
	pthread_t	pt_id;	/* thread id 	*/
	int			idx;	/* thread index */
	unsigned int	seed;	/* random seed 	*/
} thr_arg[MAX_THREAD];

struct share_data {
	int			i_data;	/* shared data for mutex example */
	pthread_mutex_t	p_mutex;
	pthread_mutexattr_t	p_mutexattr;
} *p_shr_data;

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);

int main()
{
	int		i;
	struct timeval tv;

	/* init var, random seed */
	memset(thr_arg, 0x00, sizeof(struct thread_arg) * MAX_THREAD);
	if ((p_shr_data = calloc(1, sizeof(struct share_data))) == NULL) {
		printf("[FAIL] calloc()\n"); exit(EXIT_FAILURE);
	};
	memset(p_shr_data, 0x00, sizeof(struct share_data));
	gettimeofday(&tv, NULL);
	srandom((unsigned int)tv.tv_usec);

	pthread_mutex_init(&p_shr_data->p_mutex, NULL);

	/* Start thread */
	for(i=0; i<MAX_THREAD; i++) {
		thr_arg[i].idx = i;
		if (pthread_create(&thr_arg[i].pt_id, NULL, start_thread, (void *)&thr_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create()\n");
			return 0;
		}
		printf("[MAIN] Create thread : tid = %lu\n", thr_arg[i].pt_id);
	}

	printf("[MAIN] Will join All threads!\n");
	clean_thread(thr_arg);
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
	int		i, cnt = 0, ret;

	while ( (ret = pthread_mutex_trylock(&p_shr_data->p_mutex))) {
		/* already locked */
		if (ret == EBUSY) {
			printf("[TRYLOCK] EBUSY, Thread(%lu)-counter(%d)\n", t_arg->pt_id, cnt++);
		} else {
			printf("[TRYLOCK] error(%d), Thread(%lu)\n", ret, t_arg->pt_id);
		}
		sleep(1); /* killing time ! */
	}

	gettimeofday(&tv, NULL);
	t_arg->seed = (int) tv.tv_usec;
	printf("\t[Thread] No.%d) Get seed(%d)\n", t_arg->idx, t_arg->seed);
	printf("\t\tcurrent child pid is %d\n", getpid());
	printf("\t\twill sleep %d sec.\n", i = rand_r(&t_arg->seed) % 7);
	sleep(i);
	p_shr_data->i_data = t_arg->seed;	/* update data */
	if ((ret = pthread_mutex_unlock(&p_shr_data->p_mutex))) {
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
		printf("+ (%d) Thread id(%lu), seed(%d)\n", t_arg->idx, t_arg->pt_id, t_arg->seed);
	}
	printf("[Clean] All thread are joined.\n");
}
