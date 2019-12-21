/* vim: set ts=4 sw=4: */
/* Filename    : pthread_barrier.c
 * Description : pthread with barrier
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#ifdef _XOPEN_SOURCE
# if (_XOPEN_SOURCE - 0) <= 500
# undef _XOPEN_SOURCE
# endif
#endif
#define _XOPEN_SOURCE 600
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#define MAX_THREAD	5

pthread_barrier_t	pt_barrier;	/* barrier */
struct thread_arg {
	pthread_t	pt_id;	/* thread id 	*/
	int			idx;	/* thread index */
} thr_arg[MAX_THREAD];

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);
#define GET_TIME0(a)	get_time0(a, sizeof(a))
inline char * get_time0(char *buf, size_t sz_buf);

int main()
{
	int		i;

	/* initialize barrier */
	/* int pthread_barrier_init(pthread_barrier_t *restrict barrier,
	                 const pthread_barrierattr_t *restrict attr, unsigned count); 
	 */
	pthread_barrier_init(&pt_barrier, NULL, MAX_THREAD);

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
	pthread_barrier_destroy(&pt_barrier);
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
	int		seed, ret;
	char	ts_now[20];

	gettimeofday(&tv, NULL);
	seed = (int) tv.tv_usec % 7;
	printf("\t[Thread] No.%d) [%s] wait on barrier after sleep %d sec.\n", 
			t_arg->idx, GET_TIME0(ts_now), seed);
	sleep(seed);
	if ((ret = pthread_barrier_wait(&pt_barrier)) == EINVAL) {
		printf("\t errno : %d\n", ret); return NULL;
	}
	printf("\t[Thread] No.%d) [%s] wake up.\n", t_arg->idx, GET_TIME0(ts_now));

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
		printf("+ (%d) Thread id(%lu)\n", t_arg->idx, t_arg->pt_id);
	}
	printf("[Clean] All thread are joined.\n");
}


/* Name : get_time0
 * Desc : print current time with UNIX time stamp
 * Argv : None
 * Ret  : Not null(if success) NULL(fail to convert)
 */
inline char * get_time0(char *buf, size_t sz_buf)
{
/* #define STR_TIME_FORMAT		"%y-%m-%d/%H:%M:%S" */
#define STR_TIME_FORMAT		"%H:%M:%S"
	time_t	t0;
	struct tm	tm_now;

	if (buf == NULL) return NULL;
	if (time(&t0) == ((time_t)-1)) {
		return NULL;
	}
	localtime_r(&t0, &tm_now);

	if (strftime(buf, sz_buf, STR_TIME_FORMAT, &tm_now) == 0) {
		return NULL;
	}

	return buf;
}
