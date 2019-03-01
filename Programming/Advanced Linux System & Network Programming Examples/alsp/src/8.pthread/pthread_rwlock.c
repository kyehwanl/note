/* vim: set ts=4 sw=4: */
/* Filename    : pthread_rwlock.c
 * Description : pthread with reader-writer locks
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
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#define MAX_THREAD	5

struct thread_arg {
	pthread_t	pt_id;	/* thread id 	*/
	int			idx;	/* thread index */
} thr_arg[MAX_THREAD];

struct share_data {
	int			i_data;	/* shared data for rwlock example */
	pthread_rwlock_t 		rwlock_data;
	pthread_rwlockattr_t	rwlockattr_data;
} *p_shr_data;

void *start_thread(void *);			/* thread start function */
void clean_thread(struct thread_arg *);
#define GET_TIME0(a)	get_time0(a, sizeof(a))
inline char * get_time0(char *buf, size_t sz_buf);

int main()
{
	int		i;

	/* init var, random seed */
	memset(thr_arg, 0x00, sizeof(struct thread_arg) * MAX_THREAD);
	if ((p_shr_data = calloc(1, sizeof(struct share_data))) == NULL) {
		printf("[FAIL] calloc()\n"); exit(EXIT_FAILURE);
	};

	/* initialize rwlock */
#ifdef USE_PSHARED
	pthread_rwlockattr_init(&p_shr_data->rwlockattr_data);
	pthread_rwlockattr_setpshared(&p_shr_data->rwlockattr_data, PTHREAD_PROCESS_PRIVATE);
	pthread_rwlock_init(&p_shr_data->rwlock_data, (const pthread_rwlockattr_t *) &p_shr_data->rwlockattr_data);
#else
	pthread_rwlockattr_init(&p_shr_data->rwlockattr_data);
	pthread_rwlockattr_setkind_np(&p_shr_data->rwlockattr_data, PTHREAD_RWLOCK_PREFER_WRITER_NP);
	//pthread_rwlock_init(&p_shr_data->rwlock_data, NULL);
	pthread_rwlock_init(&p_shr_data->rwlock_data, (const pthread_rwlockattr_t *) &p_shr_data->rwlockattr_data);
#endif

	/* Start thread */
	for(i=0; i<MAX_THREAD; i++) {
		thr_arg[i].idx = i;
		if (pthread_create(&thr_arg[i].pt_id, NULL, start_thread, (void *)&thr_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create()\n");
			return 0;
		}
	}
	printf("[MAIN] Will join All threads!\n");
	clean_thread(thr_arg);
	pthread_rwlock_destroy(&p_shr_data->rwlock_data);
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
	int		i, ret;
	char	ts_now[20];

	for (i=0; i<2; i++) {
		/* 1st thread use writer lock, otherwise use reader lock. */
		if (t_arg->idx == 0) {
			printf("[Thread:%d,%s] Trying to get wrlock\n", t_arg->idx, GET_TIME0(ts_now));
			if ((ret = pthread_rwlock_wrlock(&p_shr_data->rwlock_data))) {
				printf("\t errno : %d\n", ret); return NULL;
			}
			printf("[Thread:%d] Get wrlock. Inc 1 for data, then sleep 1 sec.\n", 
					t_arg->idx);
			p_shr_data->i_data++;
			sleep(1);
			printf("[Thread:%d,%s] release wrlock.\n", t_arg->idx, GET_TIME0(ts_now));
			if ((ret = pthread_rwlock_unlock(&p_shr_data->rwlock_data))) {
				printf("\t errno : %d\n", ret); return NULL;
			}
		} else {
			printf("\t[Thread:%d,%s] Trying to get rdlock\n", t_arg->idx, GET_TIME0(ts_now));
			if ((ret = pthread_rwlock_rdlock(&p_shr_data->rwlock_data))) {
				printf("\t errno : %d\n", ret); return NULL;
			}
			printf("\t[Thread:%d] Get rdlock. Data=%d, sleep 2 sec.\n", 
					t_arg->idx, p_shr_data->i_data);
			sleep(2);
			printf("\t[Thread:%d,%s] release rdlock.\n", t_arg->idx, GET_TIME0(ts_now));
			if ((ret = pthread_rwlock_unlock(&p_shr_data->rwlock_data))) {
				printf("\t errno : %d\n", ret); return NULL;
			}
		}
		sleep(1);
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
