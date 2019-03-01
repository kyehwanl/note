/* vim: set ts=4 sw=4: */
/* Filename    : sig_pending.c
 * Description : simple signal model with suspend/mask/pending
 * Author      : SunYoung Kim <sunyzero@yahoo.com>
 * Notes       : sigaction & sigprocmask, sigsuspend, sigpending
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void chk_sigusr(int signum);

int main()
{
	struct sigaction sa_usr1;
	sigset_t		sigset_mask, sigset_oldmask;
	sigset_t		sigset_pend;

	sa_usr1.sa_handler = chk_sigusr;
	sigfillset(&sa_usr1.sa_mask); /* blocking signal. It can't be allowed interruption by other signal */
	sa_usr1.sa_flags = 0;

	sigaction(SIGUSR1, &sa_usr1, NULL);

	sigfillset(&sigset_mask);
	sigdelset(&sigset_mask, SIGINT);
	printf("[MAIN] pid is %d\n", getpid());
	for(;;) {
		sigprocmask(SIG_SETMASK, &sigset_mask, &sigset_oldmask);
		printf("[MAIN] Blocking all signal except SIGINT\n");
		sleep(5);

		sigpending(&sigset_pend); /* check blocked signal */
		if (sigismember(&sigset_pend, SIGTERM)) {
			printf("[MAIN-SIGNAL] Recv SIGTERM\n");
			exit(0);
		} else {
			printf("[MAIN-SIGNAL] Recv other signal\n");
		}
		sigprocmask(SIG_SETMASK, &sigset_oldmask, NULL); /* 블럭킹 해제 */
		
		printf("[MAIN] Recv SIGNAL...\n");
	}
}

/* Name : signal handler func about SIGUSR1 & SIGUSR2
 * Desc : sleep 10 sec & print message every second.
 * Argv : 
 * Ret  : None
 */
void chk_sigusr(int signum)
{
	int	i;
	for (i=0; i<10; i++) {
		printf("\tSignal(%s):%d sec.\n", signum == SIGUSR1 ? "USR1":"USR2", i);
		sleep(1);
	}
}
