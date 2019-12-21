/* vim: set ts=4 sw=4: */
/* Filename    : pthread_signal.c
 * Description : pthread signal processor model
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : signal thread
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>

#define MAX_THREAD	5

void *start_sigthread(void *);		/* signal thread */
void *start_thread(void *);			/* thread start function */
struct thread_arg {
	pthread_t	pt_id;
	void *(*func)();
} thr_arg[MAX_THREAD] = {
	{0, start_sigthread},
	{0, start_thread},
	{0, start_thread},
	{0, NULL}
};

void clean_thread(struct thread_arg *t_arg);

int main()
{
	int		i;
	sigset_t	sigset_mask, sigset_oldmask;

	/* init signal mask */
	sigfillset(&sigset_mask);
	sigdelset(&sigset_mask, SIGINT);
	pthread_sigmask(SIG_SETMASK, &sigset_mask, &sigset_oldmask);
	printf("[MAIN] Process PID = %d\n", getpid());

	/* Start thread */
	for(i=0; i<MAX_THREAD && thr_arg[i].func != NULL; i++) {
		if (pthread_create(&thr_arg[i].pt_id, NULL, thr_arg[i].func, (void *)&thr_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create()\n");
			return 0;
		}
		printf("[MAIN] Create thread : tid = %lu\n", thr_arg[i].pt_id);
	}
	printf("[MAIN] Will join All threads!\n");
	clean_thread(thr_arg);
	return 0;
}

/* Name : start_sigthread
 * Desc : thread function to process signal
 * Argv : int idx (child index)
 * Ret  : None
 */
void *start_sigthread(void *arg)
{
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	sigset_t	sigset_mask;
	int		signum;

	printf("* Start signal thread (id = %lu)\n", (long)pthread_self());
	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGUSR1);
	sigaddset(&sigset_mask, SIGTERM);
	while (1) {
		if (sigwait(&sigset_mask, &signum)) {
			fprintf(stdout, "[SIGNAL] FAIL: sigwait()\n");
		}
		switch(signum) {
			case SIGUSR1:
				printf("[SIGNAL] Received SIGUSR1\n");
				break;
			case SIGTERM:
			case SIGINT:
				printf("[SIGNAL] Received SIGTERM or SIGINT\n");
				exit(EXIT_SUCCESS);
			default:
				fprintf(stdout, "[SIGNAL] unknown signal (%d)\n", signum);
				break;
		}
	}
	return t_arg;
}

/* Name : start_thread
 * Desc : thread function with message
 * Argv : int idx (child index)
 * Ret  : None
 */
void *start_thread(void *arg)
{
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	int		i;

	for(i=0;;i++) {
		printf("\t[THREAD:%lu] i = %d\n", t_arg->pt_id, i);
		sleep(2);
	}
	return t_arg;
}

/* Name : clean_thread
 * Desc : clean(join) thread
 * Argv : None
 * Ret  : None
 */
void clean_thread(struct thread_arg *t_arg)
{
	int         i;
	struct thread_arg *t_arg_ret;

	for (i=0; i<MAX_THREAD && t_arg->func != NULL; i++, t_arg++) {
		pthread_join(t_arg->pt_id, (void **)&t_arg_ret);
		printf("+ Thread id(%lu)\n", t_arg->pt_id);
	}
	printf("[Clean] All thread are joined.\n");
}
