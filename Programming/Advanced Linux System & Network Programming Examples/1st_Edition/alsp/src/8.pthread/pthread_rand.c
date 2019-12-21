/* vim: set ts=4 sw=4: */
/* Filename    : pthread_rand.c
 * Description : simple pthread model
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : joinable thread
 */
/* reentrant declaration */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


#define MAX_THREAD	5

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
	srand((unsigned int)tv.tv_usec);

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
	int		i;

	gettimeofday(&tv, NULL);
	t_arg->seed = (int) tv.tv_usec;
	printf("\t[Thread] No.%d) Get seed(%d)\n", t_arg->idx, t_arg->seed);
	printf("\t\tcurrent child pid is %d\n", getpid());
	printf("\t\twill sleep %d sec.\n", i = rand_r(&t_arg->seed) % 7);
	sleep(i);

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
