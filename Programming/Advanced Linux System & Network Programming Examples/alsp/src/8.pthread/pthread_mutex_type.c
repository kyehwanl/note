/* vim: set ts=4 sw=4: */
/* Filename    : pthread_mutex_type.c
 * Description : example of pthread mutex
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#define _XOPEN_SOURCE 500
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#define MAX_THREAD	3

/* statically initialize mutex */
pthread_mutex_t		mutex_thr;
pthread_mutexattr_t	mutexattr_thr;
int	i_data;				/* shared data for mutex example */

struct thread_arg {
	pthread_t	pt_id;	/* thread id 	*/
	int			idx;	/* thread index */
	unsigned int	seed;	/* random seed 	*/
} thr_arg[MAX_THREAD];

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);

int main()
{
	int		i;
	struct timeval tv;

	/* init var, random seed */
	memset(thr_arg, 0x00, sizeof(struct thread_arg) * MAX_THREAD);
	gettimeofday(&tv, NULL);
	srandom((unsigned int)tv.tv_usec);

	pthread_mutexattr_init(&mutexattr_thr); /* init mutex attribute var */
#if defined(NORMAL_MUTEX) || defined(TIMED_MUTEX)
	pthread_mutexattr_settype(&mutexattr_thr, PTHREAD_MUTEX_NORMAL); /* timed mutex */
#elif defined(RECURSIVE_MUTEX)
	pthread_mutexattr_settype(&mutexattr_thr, PTHREAD_MUTEX_RECURSIVE);
#elif defined(ERRORCHECK_MUTEX)
	pthread_mutexattr_settype(&mutexattr_thr, PTHREAD_MUTEX_ERRORCHECK);
#endif
	pthread_mutex_init(&mutex_thr,(const pthread_mutexattr_t *) &mutexattr_thr);

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
	int		i, ret;

	/* lock mutex twice -> there are something wrong! */
	if ((ret = pthread_mutex_lock(&mutex_thr))) {
		if (ret == EDEADLK)
			printf("\t EDEADLK detected\n");
		else
			printf("\t errno : %d\n", ret);
	}
	if ((ret = pthread_mutex_lock(&mutex_thr))) {
		if (ret == EDEADLK)
			printf("\t EDEADLK detected\n");
		else
			printf("\t errno : %d\n", ret);
	}
	gettimeofday(&tv, NULL);
	t_arg->seed = (int) tv.tv_usec;
	printf("\t[Thread] No.%d) Get seed(%d)\n", t_arg->idx, t_arg->seed);
	printf("\t\tcurrent child pid is %d\n", getpid());
	printf("\t\twill sleep %d sec.\n", i = rand_r(&t_arg->seed) % 7);
	sleep(i);
	i_data = t_arg->seed; /* update data */
	if ((ret = pthread_mutex_unlock(&mutex_thr))) {
		printf("Error code: %d\n", ret);
	}

	return t_arg; /* an alternative func, pthread_exit() */
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
