/* vim: set ts=4 sw=4: */
/* Filename    : rt_timer.c
 * Description : RT timer
 * Author      : Sun Young Kim <sunyzero@yahoo.com>
 * Notes       : 
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define SIGNO_RTTIMER	SIGRTMIN
#define GET_TIME0(a)	get_time0(a, sizeof(a)) == NULL ? "error" : a
inline char * get_time0(char *buf, size_t sz_buf);

int inst_timer(void);
void h_rt_timer(int signum, siginfo_t *si, void *sv);

int main(int argc, char *argv[])
{
	sigset_t	sigset_timer;
	if (inst_timer() == -1) {
		return EXIT_FAILURE;
	}

	sigemptyset(&sigset_timer);
	while (1) {
		sigsuspend(&sigset_timer);
	}

	return 0;
}

/* Name : inst_timer
 * Desc : install timer handler with RT signal
 * Argv : None
 * Ret  : 0 (if success), -1 (fail to convert)
 */
int inst_timer(void)
{
	struct sigaction sa_rt1;
	struct sigevent sigev;
	timer_t	rt_timer;
	struct itimerspec rt_itspec;
	char	ts_now[20];

	memset(&sa_rt1, 0, sizeof(sa_rt1));
	sigemptyset(&sa_rt1.sa_mask);
	sa_rt1.sa_sigaction = h_rt_timer; /* rt_timer handler */
	sa_rt1.sa_flags = SA_SIGINFO;

	if (sigaction(SIGNO_RTTIMER, &sa_rt1, NULL) == -1) {
		perror("FAIL: sigaction()");
		return -1;
	}
	sigev.sigev_notify = SIGEV_SIGNAL; /* notification with signal */
	sigev.sigev_signo = SIGNO_RTTIMER;

	/* create timer */
	if (timer_create(CLOCK_REALTIME, &sigev, &rt_timer) == -1) {
		perror("FAIL: timer_create()");
		return -1;
	}

	/* interval timer setting */
	rt_itspec.it_value.tv_sec = 2; /* timer expiration after 2.5 sec. */
	rt_itspec.it_value.tv_nsec = 500000000; /* 0.5 sec */
	rt_itspec.it_interval.tv_sec = 4; /* periodic timer with 4 sec. */
	rt_itspec.it_interval.tv_nsec = 0;
	printf("Enable timer at %s.\n", GET_TIME0(ts_now));
	if (timer_settime(rt_timer, 0, &rt_itspec, NULL) == -1) {
		perror("FAIL: timer_settime()");
		return -1;
	}

	return 0;
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
	struct timespec	tspec;
	struct tm	tm_now;

	if (buf == NULL) return NULL;
	if (clock_gettime(CLOCK_REALTIME, &tspec) == -1) {
		return NULL;
	}
	localtime_r((time_t *)&tspec.tv_sec, &tm_now);

	if (strftime(buf, sz_buf, STR_TIME_FORMAT, &tm_now) == 0) {
		return NULL;
	}
	return buf;
}


/* Name : h_rt_timer
 * Desc : handler for RT timer
 * Argv : None
 * Ret  : None
 */
void h_rt_timer(int signum, siginfo_t *si, void *sv)
{
	char	ts_now[20];
	printf("-> RT timer expiration at %s\n", GET_TIME0(ts_now));
}
